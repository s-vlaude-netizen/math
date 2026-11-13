/*
 * arcdfs2.c -- all pancyclic arc-length vectors of each type, covering the lengths from BOTH ends.
 *
 * Input (from `arcform.py dump K`): one type per line,
 *      idx p F  lo_0..lo_{p-1}  (m_j mask_j) x F  E  (i j) x k
 * A type has p arcs with unknown lengths a_i >= lo_i, n = sum a_i, and F cycles (every cycle of the
 * graph except the outer Hamiltonian one).  Cycle j uses m_j chords and the arcs in mask_j, so
 *      length_j = m_j + sum of its arcs,         defect_j = n - length_j = (sum of the arcs it skips) - m_j.
 * The graph is pancyclic iff the lengths take every value 3..n-1, i.e. the defects take every
 * value 1..n-3.  Short cycles use few (short) arcs, long cycles skip few (short) arcs, so both ends
 * constrain the short arcs, and nothing depends on n.
 *
 * Search.  Some arcs are assigned.  A cycle's length is known when all its arcs are assigned, its
 * defect is known when all the arcs it skips are.  Let L be the smallest length >= 3 that no known
 * length equals and D the smallest defect >= 1 that no known defect equals.  At every node ONE of two
 * complete case distinctions is used (the one with fewer children):
 *   by L:  (stop)   no cycle has length L.  Then n <= L, and every assignment of the remaining arcs
 *                   with n <= L in which no cycle has length L is a solution;
 *          (cover)  the lowest-numbered cycle with length L is j: branch over j and over the values
 *                   of its unassigned arcs;
 *   by D:  (stop)   no cycle has defect D.  Then n <= D+2, and every assignment with n <= D+2 is a
 *                   solution (a cycle with defect D would need n >= D+3);
 *          (cover)  the lowest-numbered cycle with defect D is j: branch over j and over the values
 *                   of the unassigned arcs it skips.
 * "Lowest-numbered" is enforced when cycles become known: a branch dies when a cycle numbered below
 * the chosen one turns out to have the same length (defect).  So every solution is produced exactly
 * once.  If either L or D has no child at all, the node has no solution.
 *
 * With NMIN > 0 only n >= NMIN is wanted.  A node is cut when n >= NMIN is impossible by counting:
 * n-3 different lengths are needed and the cycles of unknown length that share the same unassigned
 * arcs and the same partial sum will have equal lengths; the same for defects.
 * With --max NMAX only n <= NMAX is wanted.
 *
 * Every solution is re-verified from scratch before it is counted (abort on failure).
 *
 * Usage: arcdfs2 NMIN [--print] [--max NMAX] [--only-L] < types      (NMIN = 0: all solutions)
 *        --print: one line per solution,  S idx n a_0..a_{p-1} C u,v u,v ...   (arc lengths, then the
 *        chords on the cycle 0..n-1 with position 0 at vertex 0), and one line  T idx nodes=.. solutions=..
 *        per type searched
 *        --only-L never branches by D (this is the algorithm of arcdfs.c; for comparison)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXP 20
#define MAXF 1100
#define MAXL 2200
#define HSZ 4096

static int p, F, lo[MAXP], fm[MAXF];
static int nE, Ei[32], Ej[32];                     /* the chords of the type, as pairs of positions */
static unsigned A[MAXF], C[MAXF], FULL;
static int val[MAXP];
static unsigned AM;                               /* assigned arcs */
static int ls[MAXF], ds[MAXF];                    /* partial length / partial defect of each cycle */
static int cntL[MAXL], cntD[MAXL];                /* cycles with this known length / defect */
static int chosenL[MAXL], chosenD[MAXL];          /* cycle branched on for this length / defect, or -1 */
static int distL, distD;                          /* number of different known lengths / defects >= 1 */
static int NMIN, NMAX = 1 << 30, PRINT, ONLYL, typeidx;
static long long nodes, nsol, hist[MAXL], nodesL, nodesD;
static int bestn;
static unsigned hkey[HSZ];
static int hgen[HSZ], gen;

typedef struct { int nl, nd, doneL[MAXF], doneD[MAXF], addL[MAXF], addD[MAXF]; unsigned AMold; } Undo;

static int sumval(unsigned m)
{
    int s = 0;
    while (m) { int i = __builtin_ctz(m); s += val[i]; m &= m - 1; }
    return s;
}

/* The arcs in `newm` have just received values.  Returns 0 if the assignment must be rejected:
   a cycle gets length `forbidL`, or a length/defect that was branched on with a higher-numbered cycle. */
static int apply(unsigned newm, int forbidL, Undo *u)
{
    int ok = 1;
    u->AMold = AM; AM |= newm; u->nl = u->nd = 0;
    for (int f = 0; f < F; f++) {
        unsigned hit = A[f] & newm;
        u->addL[f] = 0; u->addD[f] = 0;
        if (hit) {
            int s = sumval(hit);
            u->addL[f] = s; ls[f] += s;
            if ((A[f] & ~AM) == 0) {
                int v = ls[f];
                if (v >= MAXL) { fprintf(stderr, "length overflow\n"); exit(3); }
                if (v == forbidL || chosenL[v] > f) ok = 0;
                if (cntL[v]++ == 0) distL++;
                u->doneL[u->nl++] = v;
            }
        }
        hit = C[f] & newm;
        if (hit) {
            int s = sumval(hit);
            u->addD[f] = s; ds[f] += s;
            if ((C[f] & ~AM) == 0) {
                int v = ds[f];
                if (v < 0 || v >= MAXL) { fprintf(stderr, "defect out of range\n"); exit(3); }
                if (v >= 1) {
                    if (chosenD[v] > f) ok = 0;
                    if (cntD[v]++ == 0) distD++;
                }
                u->doneD[u->nd++] = v;
            }
        }
    }
    return ok;
}

static void revert(Undo *u)
{
    for (int q = 0; q < u->nl; q++) if (--cntL[u->doneL[q]] == 0) distL--;
    for (int q = 0; q < u->nd; q++) { int v = u->doneD[q]; if (v >= 1 && --cntD[v] == 0) distD--; }
    for (int f = 0; f < F; f++) { ls[f] -= u->addL[f]; ds[f] -= u->addD[f]; }
    AM = u->AMold;
}

static void solution(void)
{
    static char seen[MAXL];
    int n = 0;
    for (int i = 0; i < p; i++) {
        if (val[i] < lo[i]) { fprintf(stderr, "type %d: arc below its lower bound\n", typeidx); exit(3); }
        n += val[i];
    }
    /* independent re-verification */
    memset(seen, 0, (size_t)n + 1);
    for (int f = 0; f < F; f++) { int v = fm[f] + sumval(A[f]); if (v > n) { fprintf(stderr, "cycle longer than n\n"); exit(3); } seen[v] = 1; }
    for (int v = 3; v < n; v++) if (!seen[v]) { fprintf(stderr, "type %d: non-pancyclic vector reported\n", typeidx); exit(3); }
    if (n < NMIN || n > NMAX) return;
    nsol++; if (n < MAXL) hist[n]++; if (n > bestn) bestn = n;
    if (PRINT) {
        int pos[MAXP + 1];
        pos[0] = 0;
        for (int i = 0; i < p; i++) pos[i + 1] = pos[i] + val[i];
        printf("S %d %d", typeidx, n);
        for (int i = 0; i < p; i++) printf(" %d", val[i]);
        if (nE) { printf(" C"); for (int c = 0; c < nE; c++) printf(" %d,%d", pos[Ei[c]], pos[Ej[c]]); }
        printf("\n");
    }
}

static void dfs(void);

/* Give values to arcs[i..t-1].  mode 0: they must sum to exactly `left`, then recurse;
   mode 1: they may sum to at most `left`, and every such assignment that apply() accepts is a solution. */
static void distribute(const int *arcs, int t, int i, int left, int minrest, unsigned newm, int forbidL, int mode)
{
    if (i == t) {
        if (mode == 0 && left != 0) return;
        Undo *u = malloc(sizeof(Undo));
        if (apply(newm, forbidL, u)) { if (mode == 0) dfs(); else solution(); }
        revert(u);
        free(u);
        return;
    }
    int a = arcs[i];
    int hi = left - (minrest - lo[a]);
    int from = lo[a];
    if (mode == 0 && i == t - 1) from = hi;          /* the last arc is forced */
    for (int v = from; v <= hi; v++) {
        val[a] = v;
        distribute(arcs, t, i + 1, left - v, minrest - lo[a], newm, forbidL, mode);
    }
    val[a] = 0;
}

/* number of ways to split `extra` units over t parts, capped */
static double ways(int extra, int t)
{
    if (extra < 0) return 0;
    double w = 1;
    for (int i = 1; i < t; i++) { w = w * (extra + i) / i; if (w > 1e15) return 1e15; }
    return w;
}

static int minsum(unsigned m)
{
    int s = 0;
    while (m) { int i = __builtin_ctz(m); s += lo[i]; m &= m - 1; }
    return s;
}

/* number of different (unassigned part, partial sum) among the cycles whose value is still unknown */
static int classes(const unsigned *M, const int *part)
{
    int c = 0;
    gen++;
    for (int f = 0; f < F; f++) {
        unsigned U = M[f] & ~AM;
        if (!U) continue;
        unsigned key = U * 2654435761u + (unsigned)(part[f] + 4096) * 40503u;
        unsigned full = (U << 13) ^ (unsigned)(part[f] + 4096);
        unsigned h = key >> 20;                      /* 12 bits */
        for (;;) {
            if (hgen[h] != gen) { hgen[h] = gen; hkey[h] = full; c++; break; }
            if (hkey[h] == full) break;
            h = (h + 1) & (HSZ - 1);
        }
    }
    return c;
}

static void dfs(void)
{
    nodes++;
    int L = 3, D = 1;
    while (cntL[L] > 0) L++;
    while (cntD[D] > 0) D++;
    int total = 0;
    for (int i = 0; i < p; i++) total += val[i];
    if (AM == FULL) { if (L >= total) solution(); return; }

    unsigned FREE = FULL & ~AM;
    int free_arcs[MAXP], nfree = 0, minfree = 0;
    for (int i = 0; i < p; i++) if (FREE >> i & 1) { free_arcs[nfree++] = i; minfree += lo[i]; }
    if (total + minfree > NMAX) return;              /* every completion has n > NMAX */

    if (NMIN) {
        if (3 + distL + classes(A, ls) < NMIN) return;
        if (!ONLYL && 3 + distD + classes(C, ds) < NMIN) return;
    }

    /* children by L */
    int stopL = (L >= NMIN && total + minfree <= L);
    double costL = stopL ? ways(L - total - minfree, nfree + 1) : 0;
    for (int j = 0; j < F; j++) {
        unsigned U = A[j] & ~AM;
        if (U) costL += ways(L - ls[j] - minsum(U), __builtin_popcount(U));
    }
    if (costL == 0) return;
    int useD = 0, stopD = 0;
    if (!ONLYL) {
        stopD = (D + 2 >= NMIN && total + minfree <= D + 2);
        double costD = stopD ? ways(D + 2 - total - minfree, nfree + 1) : 0;
        for (int j = 0; j < F; j++) {
            unsigned U = C[j] & ~AM;
            if (U) costD += ways(D - ds[j] - minsum(U), __builtin_popcount(U));
        }
        if (costD == 0) return;
        useD = costD < costL;
    }

    if (!useD) {
        nodesL++;
        if (stopL) distribute(free_arcs, nfree, 0, L - total, minfree, FREE, L, 1);
        for (int j = 0; j < F; j++) {
            unsigned U = A[j] & ~AM;
            if (!U) continue;
            int arcs[MAXP], t = 0, minU = 0;
            for (int i = 0; i < p; i++) if (U >> i & 1) { arcs[t++] = i; minU += lo[i]; }
            int resid = L - ls[j];
            if (resid < minU) continue;
            chosenL[L] = j;
            distribute(arcs, t, 0, resid, minU, U, -1, 0);
        }
        chosenL[L] = -1;
    } else {
        nodesD++;
        if (stopD) distribute(free_arcs, nfree, 0, D + 2 - total, minfree, FREE, -1, 1);
        for (int j = 0; j < F; j++) {
            unsigned U = C[j] & ~AM;
            if (!U) continue;
            int arcs[MAXP], t = 0, minU = 0;
            for (int i = 0; i < p; i++) if (U >> i & 1) { arcs[t++] = i; minU += lo[i]; }
            int resid = D - ds[j];
            if (resid < minU) continue;
            chosenD[D] = j;
            distribute(arcs, t, 0, resid, minU, U, -1, 0);
        }
        chosenD[D] = -1;
    }
}

int main(int argc, char **argv)
{
    NMIN = argc > 1 ? atoi(argv[1]) : 0;
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--print")) PRINT = 1;
        else if (!strcmp(argv[i], "--only-L")) ONLYL = 1;
        else if (!strcmp(argv[i], "--max") && i + 1 < argc) NMAX = atoi(argv[++i]);
        else { fprintf(stderr, "usage: arcdfs2 NMIN [--print] [--max NMAX] [--only-L] < types\n"); return 2; }
    }
    static char line[1 << 17];
    long long types = 0, searched = 0, total_nodes = 0, total_sol = 0, maxnodes = 0, tl = 0, td = 0;
    int overall = 0;
    memset(chosenL, -1, sizeof chosenL); memset(chosenD, -1, sizeof chosenD);
    while (fgets(line, sizeof line, stdin)) {
        char *q = line;
        typeidx = (int)strtol(q, &q, 10); p = (int)strtol(q, &q, 10); F = (int)strtol(q, &q, 10);
        if (p <= 0 || p > MAXP || F >= MAXF) { fprintf(stderr, "type %d too large\n", typeidx); return 1; }
        for (int i = 0; i < p; i++) lo[i] = (int)strtol(q, &q, 10);
        FULL = (1u << p) - 1;
        for (int f = 0; f < F; f++) {
            fm[f] = (int)strtol(q, &q, 10); A[f] = (unsigned)strtoul(q, &q, 10); C[f] = FULL & ~A[f];
            if (A[f] & ~FULL) { fprintf(stderr, "type %d: bad mask\n", typeidx); return 1; }
            if (C[f] == 0) { fprintf(stderr, "type %d: a cycle uses every arc\n", typeidx); return 1; }
        }
        nE = 0;
        while (*q == ' ') q++;
        if (*q == 'E') {
            q++;
            for (;;) {
                char *e; long i = strtol(q, &e, 10);
                if (e == q) break;
                q = e; long j = strtol(q, &e, 10);
                if (e == q || nE >= 32) { fprintf(stderr, "type %d: bad chord list\n", typeidx); return 1; }
                q = e; Ei[nE] = (int)i; Ej[nE] = (int)j; nE++;
            }
        }
        types++;
        if (NMIN && F + 3 < NMIN) continue;          /* too few cycles for n >= NMIN */
        searched++;
        AM = 0; distL = distD = 0; nodes = nodesL = nodesD = 0; nsol = 0; bestn = 0;
        memset(cntL, 0, sizeof cntL); memset(cntD, 0, sizeof cntD); memset(val, 0, sizeof val);
        for (int f = 0; f < F; f++) {
            ls[f] = fm[f]; ds[f] = -fm[f];
            if (A[f] == 0 && cntL[fm[f]]++ == 0) distL++;        /* chords only: length known from the start */
        }
        dfs();
        total_nodes += nodes; total_sol += nsol; tl += nodesL; td += nodesD;
        if (nodes > maxnodes) maxnodes = nodes;
        if (bestn > overall) overall = bestn;
        if (PRINT) { printf("T %d nodes=%lld solutions=%lld\n", typeidx, nodes, nsol); fflush(stdout); }
    }
    printf("# types=%lld searched=%lld solutions=%lld largest n=%d nodes=%lld (largest single type %lld; branched by L %lld, by D %lld)\n",
           types, searched, total_sol, overall, total_nodes, maxnodes, tl, td);
    printf("# solutions per n:");
    for (int n = 0; n < MAXL; n++) if (hist[n]) printf(" %d:%lld", n, hist[n]);
    printf("\n");
    return 0;
}
