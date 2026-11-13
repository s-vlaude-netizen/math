/*
 * arcdfs.c -- all pancyclic arc-length vectors of each type, by covering the lengths in order.
 *
 * Input (from `arcform.py dump K`): one type per line,
 *      idx p F  lo_0..lo_{p-1}  (m_j mask_j) x F  E  (i j) x k
 * A type has p arcs with unknown lengths a_i >= lo_i and F cycle forms m_j + sum_{i in mask_j} a_i
 * (every cycle except the Hamiltonian one, whose length is n = sum a_i).  The graph is pancyclic
 * iff the forms take every value 3..n-1.
 *
 * Search.  Some arcs are assigned; a form is complete when all its arcs are.  Let L be the smallest
 * length >= 3 that no complete form has.  Every solution extending the current assignment is of
 * exactly one of two kinds:
 *   (stop)   no form equals L.  Then n <= L, and since 3..L-1 are already covered, EVERY assignment
 *            of the remaining arcs with n <= L in which no form equals L is a solution;
 *   (cover)  some incomplete form equals L.  Branch over the lowest-numbered such form j and over
 *            the values of its unassigned arcs; a branch dies when a lower-numbered form that was
 *            incomplete at the time completes with the value L (that solution belongs to another
 *            branch), so every solution is produced exactly once.
 * Small lengths force small arcs, so the branching stays small, and nothing depends on n.
 *
 * With NMIN > 0 only solutions with n >= NMIN are wanted; a branch is cut when even with every
 * incomplete form supplying a new length n cannot reach NMIN.
 *
 * With --max NMAX only solutions with n <= NMAX are wanted; a branch is cut when the arcs assigned
 * so far plus the minimum of the others already exceed NMAX.
 *
 * Usage: arcdfs NMIN [--print] [--max NMAX] < types      (NMIN = 0: all solutions)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXP 20
#define MAXF 1100
#define MAXL 2200

static int p, F, lo[MAXP], fm[MAXF];
static unsigned fmask[MAXF], FULL;
static int val[MAXP];
static unsigned AM;                    /* assigned arcs */
static int fsum[MAXF];                 /* m_j + assigned arcs of form j */
static int cnt[MAXL];                  /* complete forms with this value */
static int chosen[MAXL];               /* form branched on for this length, or -1 */
static int distinctTotal;              /* number of values v with cnt[v] > 0 */
static int NMIN, NMAX = 1 << 30, PRINT, typeidx;
static long long nodes, nsol, hist[MAXL];
static int bestn;

typedef struct { int nd, done[MAXF], add[MAXF]; unsigned AMold; } Undo;

/* Arcs in `newm` have just received values.  Update every form.  Returns 0 if the assignment must be
   rejected: a form completes with value `forbid`, or with a branched length owned by a higher form. */
static int apply(unsigned newm, int L, int forbid, Undo *u)
{
    int ok = 1;
    u->AMold = AM; AM |= newm; u->nd = 0;
    for (int f = 0; f < F; f++) {
        unsigned hit = fmask[f] & newm;
        u->add[f] = 0;
        if (!hit) continue;
        int s = 0;
        for (int i = 0; i < p; i++) if (hit >> i & 1) s += val[i];
        u->add[f] = s; fsum[f] += s;
        if ((fmask[f] & ~AM) == 0) {                 /* form f is complete now */
            int v = fsum[f];
            if (v >= MAXL) { ok = 0; continue; }
            if (v == forbid) ok = 0;
            if (v <= L && chosen[v] > f) ok = 0;
            if (cnt[v]++ == 0) distinctTotal++;
            u->done[u->nd++] = v;
        }
    }
    return ok;
}

static void revert(unsigned newm, Undo *u)
{
    for (int q = 0; q < u->nd; q++) if (--cnt[u->done[q]] == 0) distinctTotal--;
    for (int f = 0; f < F; f++) fsum[f] -= u->add[f];
    AM = u->AMold;
    (void)newm;        /* val[] is NOT cleared here: the frames of distribute() own their arcs and
                          clear them after their loops; clearing here would wipe arcs whose loops
                          are still running in outer frames */
}

static void solution(void)
{
    int n = 0;
    for (int i = 0; i < p; i++) n += val[i];
    if (n < NMIN || n > NMAX) return;
    nsol++; if (n < MAXL) hist[n]++; if (n > bestn) bestn = n;
    if (PRINT) { printf("S %d %d", typeidx, n); for (int i = 0; i < p; i++) printf(" %d", val[i]); printf("\n"); }
}

static void dfs(void);

/* Give values to arcs[i..t-1].  mode 0: they must sum to exactly `left` (cover branch, then recurse);
   mode 1: they may sum to at most `left` (stop branch, every such assignment is a solution). */
static void distribute(const int *arcs, int t, int i, int left, int minrest, unsigned newm, int L, int mode)
{
    if (i == t) {
        if (mode == 0 && left != 0) return;
        Undo *u = malloc(sizeof(Undo));
        int ok = apply(newm, L, mode == 1 ? L : -1, u);
        if (ok) { if (mode == 0) dfs(); else solution(); }
        revert(newm, u);
        free(u);
        return;
    }
    int a = arcs[i];
    int hi = left - (minrest - lo[a]);
    int from = lo[a];
    if (mode == 0 && i == t - 1) from = hi;          /* the last arc is forced */
    for (int v = from; v <= hi; v++) {
        val[a] = v;
        distribute(arcs, t, i + 1, left - v, minrest - lo[a], newm, L, mode);
    }
    val[a] = 0;
}

static void dfs(void)
{
    nodes++;
    int L = 3;
    while (cnt[L] > 0) L++;
    int total = 0;
    for (int i = 0; i < p; i++) total += val[i];
    if (AM == FULL) { if (L >= total) solution(); return; }

    int free_arcs[MAXP], nfree = 0, minfree = 0;
    for (int i = 0; i < p; i++) if (!(AM >> i & 1)) { free_arcs[nfree++] = i; minfree += lo[i]; }

    if (total + minfree > NMAX) return;              /* every completion has n > NMAX */

    /* stop: n <= L and no form takes the value L */
    if (L >= NMIN && total + minfree <= L)
        distribute(free_arcs, nfree, 0, L - total, minfree, FULL & ~AM, L, 1);

    if (NMIN) {
        int R = 0;
        for (int f = 0; f < F; f++) if (fmask[f] & ~AM) R++;
        if (L + (distinctTotal - (L - 3)) + R < NMIN) return;
    }
    /* cover: some incomplete form equals L */
    for (int j = 0; j < F; j++) {
        unsigned U = fmask[j] & ~AM;
        if (!U) continue;
        int arcs[MAXP], t = 0, minU = 0;
        for (int i = 0; i < p; i++) if (U >> i & 1) { arcs[t++] = i; minU += lo[i]; }
        int resid = L - fsum[j];
        if (resid < minU) continue;
        chosen[L] = j;
        distribute(arcs, t, 0, resid, minU, U, L, 0);
    }
    chosen[L] = -1;
}

int main(int argc, char **argv)
{
    NMIN = argc > 1 ? atoi(argv[1]) : 0;
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--print")) PRINT = 1;
        else if (!strcmp(argv[i], "--max") && i + 1 < argc) NMAX = atoi(argv[++i]);
        else { fprintf(stderr, "usage: arcdfs NMIN [--print] [--max NMAX] < types\n"); return 2; }
    }
    static char line[1 << 17];
    long long types = 0, searched = 0, total_nodes = 0, total_sol = 0, maxnodes = 0;
    int overall = 0;
    memset(chosen, -1, sizeof chosen);
    while (fgets(line, sizeof line, stdin)) {
        char *q = line;
        typeidx = (int)strtol(q, &q, 10); p = (int)strtol(q, &q, 10); F = (int)strtol(q, &q, 10);
        if (p <= 0 || p > MAXP || F >= MAXF) { fprintf(stderr, "type %d too large\n", typeidx); return 1; }
        for (int i = 0; i < p; i++) lo[i] = (int)strtol(q, &q, 10);
        for (int f = 0; f < F; f++) { fm[f] = (int)strtol(q, &q, 10); fmask[f] = (unsigned)strtoul(q, &q, 10); }
        FULL = (1u << p) - 1;
        types++;
        if (NMIN && F + 3 < NMIN) continue;          /* too few cycles for n >= NMIN */
        searched++;
        AM = 0; distinctTotal = 0; nodes = 0; nsol = 0; bestn = 0;
        memset(cnt, 0, sizeof cnt); memset(val, 0, sizeof val);
        for (int f = 0; f < F; f++) {
            fsum[f] = fm[f];
            if (fmask[f] == 0 && cnt[fm[f]]++ == 0) distinctTotal++;   /* chords only: complete from the start */
        }
        dfs();
        total_nodes += nodes; total_sol += nsol;
        if (nodes > maxnodes) maxnodes = nodes;
        if (bestn > overall) overall = bestn;
    }
    printf("# types=%lld searched=%lld solutions=%lld largest n=%d nodes=%lld (largest single type %lld)\n",
           types, searched, total_sol, overall, total_nodes, maxnodes);
    printf("# solutions per n:");
    for (int n = 0; n < MAXL; n++) if (hist[n]) printf(" %d:%lld", n, hist[n]);
    printf("\n");
    return 0;
}
