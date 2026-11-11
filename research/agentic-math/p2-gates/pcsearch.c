/*
 * pcsearch.c -- exhaustive search for pancyclic C_n + k chords (Erdos problem 1016).
 *
 * Version 4 of the searcher (versions 1-3: erdos1016/pancyclic{,2,3}.c), restructured
 * so that every pruning, ordering and symmetry rule is a named site which the mutation
 * harness can perturb: compile with -DMUT=<id> to build mutant <id>; MUT=0 is the real
 * program.  The MU(id, original, mutated) macro marks each site; mutants.md lists them.
 *
 * The goal is parametrised: NEED is the set of cycle lengths that must be present
 * (default: all of 3..n).  Every rule below is valid for any NEED, which is what makes
 * the planted-solution gate (--plant) possible at sizes where no real solution is known.
 *
 * Rules (each one only ever discards branches that contain no solution, or keeps at
 * least one member of every symmetry orbit):
 *   count   the cycle space of C_n + K chords has 2^(K+1)-1 non-zero elements, each
 *           contributes at most one cycle length;
 *   look    the same bound, applied inside a batch of new cycle-space elements;
 *   filter  the LAST chord (x,y) must create every missing length L, so for the two
 *           smallest missing lengths there must be a simple x-y path with L-1 edges;
 *   canon   rotate a chord of minimum span s to (0,s): search only sets containing
 *           (0,s) whose other chords have span >= s;
 *   mirror  x -> s-x fixes (0,s) and permutes the candidates; keep the set whose
 *           smallest candidate index is <= that of its mirror image.
 *
 * Usage: pcsearch N K [--find | --enum | --plant T] [--canon] [--span s] [--slice i m]
 *                     [--no-count] [--no-look] [--no-filter] [--no-mirror] [--seed x]
 *        pcsearch --spectrum-stdin        (lines "n u,v u,v ..." -> cycle lengths)
 * Exit: 10 found, 20 none (find mode); 0 otherwise; 3 internal assertion.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#ifndef MUT
#define MUT 0
#endif
#define MU(id, orig, alt) ((MUT) == (id) ? (alt) : (orig))
#define FILTER_CAP 9                       /* longest path (in edges) the filter will look for */

static int N, K;
static int NCH, chu[4096], chv[4096];
static short cidx[64][64];                 /* (u,v), u<v  ->  chord index, or -1 */
static int eu[64], ev[64];
static uint64_t vmask[64];
static uint64_t elem[256];
static int lensCount[72], distinctLens;
static uint64_t NEED;                      /* bit L set  <=>  length L is required */
static int NEEDN;
static int chosen[16];
static long long nodes = 0, nsol = 0;
static int ENUM = 0, CANON = 0, OPT_MIRROR = 1, OPT_COUNT = 1, OPT_LOOK = 1, OPT_FILTER = 1;
static int MINSPAN = 0, SLICEI = -1, SLICEM = 0;
static int cand[4096], NCAND, refl[4096], firstfree = -1;
static int found_k = -1, found[16];
static uint64_t adjm[64];
static int PLEN = 0, PREFIX[16];           /* --plant: follow exactly this path of candidates */

static uint64_t rng_s = 88172645463325252ULL;
static uint64_t rnd(void) { rng_s ^= rng_s << 13; rng_s ^= rng_s >> 7; rng_s ^= rng_s << 17; return rng_s; }

static int true_span(int u, int v) { int d = v - u; return d < N - d ? d : N - d; }
static int span(int c) { int d = chv[c] - chu[c]; return MU(16, (d < N - d ? d : N - d), d); }

/* Is this edge set a single cycle?  Returns its length, or 0. */
static int cyclen(uint64_t mask)
{
    int ne = __builtin_popcountll(mask);
    if (ne < MU(1, 3, 4)) return 0;
    uint64_t d1 = 0, d2 = 0, t = mask;     /* d1: degree >= 1, d2: degree >= 2 */
    while (t) {
        int e = __builtin_ctzll(t); t &= t - 1;
        uint64_t vm = vmask[e];
        if (vm & d2) return 0;
        d2 |= vm & d1; d1 |= vm;
    }
    if (d1 != d2) return 0;                /* 2-regular: a disjoint union of cycles */
    if (MUT == 2) return ne;
    int deg[64], nb[64][2];
    uint64_t vv = d1;
    while (vv) { int v = __builtin_ctzll(vv); vv &= vv - 1; deg[v] = 0; }
    t = mask;
    while (t) {
        int e = __builtin_ctzll(t); t &= t - 1;
        int a = eu[e], b = ev[e];
        nb[a][deg[a]++] = b; nb[b][deg[b]++] = a;
    }
    int start = eu[__builtin_ctzll(mask)], prev = -1, cur = start, steps = 0;
    do {
        int nx = (nb[cur][0] == prev) ? nb[cur][1] : nb[cur][0];
        prev = cur; cur = nx; steps++;
        if (steps > ne) return 0;
    } while (cur != start);
    return steps == ne ? ne : 0;
}

/* Register chord c as chord number j; return its fundamental cycle (path edges u..v-1 plus the chord). */
static uint64_t fundamental(int j, int c)
{
    int e = N + j;
    eu[e] = chu[c]; ev[e] = chv[c];
    vmask[e] = (1ULL << chu[c]) | (1ULL << chv[c]);
    uint64_t f = MU(46, 1ULL << e, (uint64_t)(1u << (e & 31)));
    for (int i = chu[c]; MU(3, i < chv[c], i <= chv[c]); i++) f |= 1ULL << i;
    return f;
}

/* Add the cycle-space elements that contain chord j.  Returns 1 if the branch is dead. */
static int batch(int j, uint64_t f, int *added, int *na)
{
    int lo = 1 << (j + 1), hi = lo << 1;
    int rest = MU(13, (1 << (K + 1)) - hi, 0);        /* elements still to come from later chords */
    for (int s = lo; s < hi; s++) {
        elem[s] = elem[s ^ lo] ^ f;
        int ne = __builtin_popcountll(elem[s]);
        if (ne >= 3 && ne <= MU(5, N, N - 1) && ((NEED >> ne) & 1) && lensCount[ne] == 0) {
            int L = cyclen(elem[s]);
            if (L) { lensCount[L]++; distinctLens++; added[(*na)++] = L; }
        }
        if (OPT_LOOK) {
            int missing = NEEDN - distinctLens;
            int room = MU(12, hi - 1 - s, hi - 2 - s) + rest;
            if (MU(11, missing > room, missing >= room)) return 1;
        }
    }
    return 0;
}

static void undo(const int *added, int na)
{
    for (int i = 0; i < na; i++) { lensCount[added[i]]--; distinctLens--; }
}

static void reset_lens(void)
{
    memset(lensCount, 0, sizeof lensCount);
    uint64_t H = 0; for (int i = 0; i < N; i++) H |= 1ULL << i;
    elem[0] = 0; elem[1] = H;
    lensCount[N] = 1;
    distinctLens = (int)((NEED >> N) & 1);
}

/* out: bit y set  <=>  there is a simple path with exactly `depth` edges from the start vertex to y */
static void paths(int v, int depth, uint64_t vis, uint64_t *out)
{
    if (depth == 0) { *out |= 1ULL << v; return; }
    uint64_t nb = MU(30, adjm[v] & ~vis, adjm[v]);
    while (nb) { int w = __builtin_ctzll(nb); nb &= nb - 1; paths(w, depth - 1, vis | (1ULL << w), out); }
}

static void emit(int k)
{
    int o[16];
    for (int i = 0; i < k; i++) o[i] = chosen[i];
    for (int i = 1; i < k; i++) { int x = o[i], q = i - 1; while (q >= 0 && o[q] > x) { o[q + 1] = o[q]; q--; } o[q + 1] = x; }
    for (int i = 0; i < k; i++) printf("%d,%d%s", chu[o[i]], chv[o[i]], i + 1 < k ? " " : "");
    printf("\n");
}

/* j chords are placed (chosen[0..j-1]); candidates with position >= start may follow. */
static int dfs(int j, int start)
{
    nodes++;
    int missing = NEEDN - distinctLens;
    if (missing == 0) {
        if (!ENUM) { found_k = j; memcpy(found, chosen, sizeof(int) * j); return 1; }
        if (j == K) { emit(K); nsol++; return 0; }
    }
    if (j >= K) return 0;
    if (OPT_COUNT) {
        int B = MU(9, (1 << (K + 1)), (1 << K)) - MU(10, (1 << (j + 1)), (1 << (j + 2)));
        if (MU(8, missing > B, missing >= B)) return 0;
    }

    /* filter: the last chord must close a cycle of each missing length */
    uint64_t P1[64], P2[64];
    int nf = 0;
    if (OPT_FILTER && missing > 0 && MU(33, j == K - 1, j >= K - 2)) {
        int L1 = 0, L2 = 0;
        for (int L = MU(34, 3, 4); L <= N; L++)
            if (((NEED >> L) & 1) && !lensCount[L]) { if (!L1) L1 = L; else { L2 = L; break; } }
        if (MUT == 36 && L1) L2 = L1 + 1;
        if (L1 && L1 - 1 <= FILTER_CAP) {
            for (int v = 0; v < N; v++) adjm[v] = 0;
            for (int e = 0; e < N + j; e++) {
                if (MUT == 31 && e == N) continue;
                adjm[eu[e]] |= 1ULL << ev[e]; adjm[ev[e]] |= 1ULL << eu[e];
            }
            int d1 = L1 - 1 + MU(28, 0, -1) + MU(29, 0, 1);
            for (int x = 0; x < N; x++) {
                P1[x] = 0; paths(x, d1, 1ULL << x, &P1[x]);
                if (MUT == 47) P1[x] &= 0xFFFFFFFFULL;
            }
            nf = 1;
            if (L2 && L2 - 1 <= FILTER_CAP) {
                for (int x = 0; x < N; x++) { P2[x] = 0; paths(x, L2 - 1, 1ULL << x, &P2[x]); }
                nf = 2;
            }
        }
    }

    int limit = CANON ? NCAND : NCH;
    for (int ci = start; ci < MU(24, limit, limit - 1); ci++) {
        if (PLEN && ci != PREFIX[j - 1]) continue;
        int c = CANON ? cand[ci] : ci;
        if (j == 1 && SLICEM && ci % MU(37, SLICEM, SLICEM + 1) != SLICEI) continue;
        if (CANON && OPT_MIRROR) {
            if (j == 1) { if (MU(18, refl[ci] < ci, refl[ci] <= ci)) continue; }
            else if (MU(19, refl[ci] < firstfree, refl[ci] <= firstfree)) continue;
        }
        if (MUT != 39 && j == MU(22, 1, 2)) firstfree = ci;
        if (nf >= 1 && !((P1[chu[c]] >> chv[c]) & 1)) continue;
        if (nf >= 2 && !((P2[chu[c]] >> chv[c]) & 1)) continue;
        chosen[j] = c;
        uint64_t f = fundamental(j, c);
        int added[64], na = 0;
        int dead = batch(j, f, added, &na);
        if (!dead && dfs(j + 1, ci + MU(23, 1, 2))) return 1;
        if (!(MUT == 7 && dead)) undo(added, na);
    }
    return 0;
}

static void init_graph(void)
{
    for (int i = 0; i < N - 1; i++) { eu[i] = i; ev[i] = i + 1; }
    eu[N - 1] = MU(4, 0, N - 2); ev[N - 1] = N - 1;
    for (int i = 0; i < N; i++) vmask[i] = (1ULL << eu[i]) | (1ULL << ev[i]);
    NCH = 0;
    memset(cidx, -1, sizeof cidx);
    for (int i = 0; i < N; i++)
        for (int jj = i + MU(26, 2, 3); jj < N; jj++) {
            if (MU(27, (i == 0 && jj == N - 1), 0)) continue;       /* that pair is a cycle edge */
            chu[NCH] = i; chv[NCH] = jj; cidx[i][jj] = (short)NCH; NCH++;
        }
}

/* Candidate list and mirror table for minimum span sp.  Returns the index of the forced chord (0,sp), or -1. */
static int build_span(int sp)
{
    int c0 = cidx[0][sp];
    if (c0 < 0) return -1;
    static short pos[4096];
    NCAND = 0;
    for (int c = 0; c < NCH; c++) {
        pos[c] = -1;
        if (MU(17, c != c0, 1) && (MUT == 15 ? span(c) > sp : span(c) >= sp)) { pos[c] = (short)NCAND; cand[NCAND++] = c; }
    }
    if (MUT != 21 || sp == 2) {
        int base = MU(20, sp, 0);
        for (int i = 0; i < NCAND; i++) {
            int c = cand[i];
            int a = ((base - chu[c]) % N + N) % N, b = ((base - chv[c]) % N + N) % N;
            if (a > b) { int t = a; a = b; b = t; }
            int d = cidx[a][b];
            refl[i] = (d >= 0) ? pos[d] : -1;
            if (refl[i] < 0 && OPT_MIRROR) { fprintf(stderr, "assertion: mirror image of a candidate is not a candidate\n"); exit(3); }
        }
    }
    firstfree = -1;
    return c0;
}

/* Place the forced chord and search below it. */
static int search_span(int c0)
{
    reset_lens();
    chosen[0] = c0;
    uint64_t f = fundamental(0, c0);
    int added[64], na = 0, r = 0;
    int dead = batch(0, f, added, &na);
    if (!dead) r = dfs(1, MU(25, 0, 1));
    if (MUT != 41) undo(added, na);
    return r;
}

static int run_search(void)
{
    reset_lens();
    if (!CANON) return dfs(0, 0);
    if (MU(38, K == 0, 0)) {
        if (NEEDN - distinctLens != 0) return 0;
        if (ENUM) { printf("\n"); nsol++; return 0; }
        found_k = 0; return 1;
    }
    int r = 0;
    for (int sp = 2; sp <= MU(14, N / 2, N / 2 - 1) && !r; sp++) {
        if (MINSPAN && sp != MINSPAN) continue;
        int c0 = build_span(sp);
        if (c0 < 0) continue;
        if (MUT == 41) {                        /* mutant: lens state not rebuilt between spans */
            chosen[0] = c0;
            uint64_t f = fundamental(0, c0);
            int added[64], na = 0;
            if (!batch(0, f, added, &na)) r = dfs(1, 0);
        } else r = search_span(c0);
    }
    return r;
}

/* Cycle-length set of C_N + the given chords, through the same cycle-space code. */
static uint64_t eval_set(const int *cs, int k)
{
    int saveLook = OPT_LOOK, saveK = K; uint64_t saveNeed = NEED; int saveNeedN = NEEDN;
    OPT_LOOK = 0; K = k;
    NEED = 0; for (int L = 3; L <= N; L++) NEED |= 1ULL << L;
    NEEDN = N - 2;
    reset_lens();
    for (int j = 0; j < k; j++) { int added[64], na = 0; batch(j, fundamental(j, cs[j]), added, &na); }
    uint64_t m = 0;
    for (int L = 3; L <= N; L++) if (lensCount[L] > 0) m |= 1ULL << L;
    OPT_LOOK = saveLook; K = saveK; NEED = saveNeed; NEEDN = saveNeedN;
    return m;
}

/* Planted-solution gate: T random chord sets W, goal = spectrum(W); is some symmetric image of W retained? */
static int run_plant(long T)
{
    long lost = 0, shown = 0;
    CANON = 1; ENUM = 0;
    for (long t = 0; t < T; t++) {
        int w[16];
        for (int i = 0; i < K; i++) {
            int ok;
            do { w[i] = (int)(rnd() % NCH); ok = 1; for (int q = 0; q < i; q++) if (w[q] == w[i]) ok = 0; } while (!ok);
        }
        NEED = eval_set(w, K) | (1ULL << N);
        NEEDN = __builtin_popcountll(NEED);
        int sp = N;
        for (int i = 0; i < K; i++) { int s = true_span(chu[w[i]], chv[w[i]]); if (s < sp) sp = s; }
        int retained = 0;
        int c0 = build_span(sp);
        if (c0 >= 0 && sp <= MU(14, N / 2, N / 2 - 1)) {
            static short pos[4096];
            for (int c = 0; c < NCH; c++) pos[c] = -1;
            for (int i = 0; i < NCAND; i++) pos[cand[i]] = (short)i;
            for (int g = 0; g < 2 * N && !retained; g++) {
                int tr = g % N, fl = g >= N, img[16], hasforced = 0, okimg = 1, np = 0;
                for (int i = 0; i < K && okimg; i++) {
                    int a = chu[w[i]], b = chv[w[i]];
                    if (fl) { a = (N - a) % N; b = (N - b) % N; }
                    a = (a + tr) % N; b = (b + tr) % N;
                    if (a > b) { int x = a; a = b; b = x; }
                    int c = cidx[a][b];
                    if (c < 0) { okimg = 0; break; }
                    if (c == c0 && !hasforced) { hasforced = 1; continue; }
                    if (pos[c] < 0) { okimg = 0; break; }
                    img[np++] = pos[c];
                }
                if (!okimg || !hasforced) continue;
                for (int i = 1; i < np; i++) { int x = img[i], q = i - 1; while (q >= 0 && img[q] > x) { img[q + 1] = img[q]; q--; } img[q + 1] = x; }
                PLEN = np; for (int i = 0; i < np; i++) PREFIX[i] = img[i];
                found_k = -1;
                if (np == 0) { reset_lens(); int added[64], na = 0; chosen[0] = c0; if (!batch(0, fundamental(0, c0), added, &na) && NEEDN - distinctLens == 0) retained = 1; }
                else if (search_span(c0)) retained = 1;
                PLEN = 0;
            }
        }
        if (!retained) {
            lost++;
            if (shown < 3) { shown++; printf("LOST:"); for (int i = 0; i < K; i++) printf(" %d,%d", chu[w[i]], chv[w[i]]); printf("\n"); }
        }
    }
    printf("plants=%ld lost=%ld n=%d k=%d\n", T, lost, N, K);
    return lost ? 1 : 0;
}

static void spectrum_stdin(void)
{
    char line[4096];
    while (fgets(line, sizeof line, stdin)) {
        char *p = line; int n = (int)strtol(p, &p, 10);
        if (n < 3 || n > 60) continue;
        N = n; init_graph();
        int cs[16], k = 0;
        while (*p) {
            while (*p == ' ') p++;
            if (*p < '0' || *p > '9') break;
            int u = (int)strtol(p, &p, 10); if (*p == ',') p++;
            int v = (int)strtol(p, &p, 10);
            if (u > v) { int t = u; u = v; v = t; }
            if (k < 15 && u >= 0 && v < N && cidx[u][v] >= 0) cs[k++] = cidx[u][v];
        }
        uint64_t m = eval_set(cs, k) | (1ULL << N);
        int first = 1;
        for (int L = 3; L <= N; L++) if ((m >> L) & 1) { printf("%s%d", first ? "" : " ", L); first = 0; }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    if (argc >= 2 && !strcmp(argv[1], "--spectrum-stdin")) { spectrum_stdin(); return 0; }
    if (argc < 3) { fprintf(stderr, "usage: %s N K [--find|--enum|--plant T] [--canon] ...\n", argv[0]); return 1; }
    N = atoi(argv[1]); K = atoi(argv[2]);
    long plantT = 0;
    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i], "--canon")) CANON = 1;
        else if (!strcmp(argv[i], "--enum")) ENUM = 1;
        else if (!strcmp(argv[i], "--find")) ENUM = 0;
        else if (!strcmp(argv[i], "--plant") && i + 1 < argc) plantT = atol(argv[++i]);
        else if (!strcmp(argv[i], "--seed") && i + 1 < argc) rng_s = strtoull(argv[++i], 0, 10) * 2654435761ULL + 88172645463325252ULL;
        else if (!strcmp(argv[i], "--span") && i + 1 < argc) { CANON = 1; MINSPAN = atoi(argv[++i]); }
        else if (!strcmp(argv[i], "--slice") && i + 2 < argc) { SLICEI = atoi(argv[++i]); SLICEM = atoi(argv[++i]); }
        else if (!strcmp(argv[i], "--no-mirror")) OPT_MIRROR = 0;
        else if (!strcmp(argv[i], "--no-count")) OPT_COUNT = 0;
        else if (!strcmp(argv[i], "--no-look")) OPT_LOOK = 0;
        else if (!strcmp(argv[i], "--no-filter")) OPT_FILTER = 0;
        else { fprintf(stderr, "unknown option %s\n", argv[i]); return 1; }
    }
    if (N < 3 || N + K > 62 || K > 6) { fprintf(stderr, "need 3 <= N, N+K <= 62, K <= 6\n"); return 1; }
    init_graph();
    NEED = 0; for (int L = 3; L <= N; L++) NEED |= 1ULL << L;
    NEEDN = N - 2;
    if (plantT) return run_plant(plantT);

    struct timespec t0, t1; clock_gettime(CLOCK_MONOTONIC, &t0);
    int r = run_search();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double secs = (t1.tv_sec - t0.tv_sec) + 1e-9 * (t1.tv_nsec - t0.tv_nsec);
    if (ENUM) { fprintf(stderr, "# n=%d k=%d solutions=%lld nodes=%lld time=%.2fs\n", N, K, nsol, nodes, secs); return 0; }
    printf("n=%d k=%d nodes=%lld time=%.2fs ", N, K, nodes, secs);
    if (r) {
        printf("PANCYCLIC chords=[");
        for (int i = 0; i < found_k; i++) printf("(%d,%d)%s", chu[found[i]], chv[found[i]], i + 1 < found_k ? "," : "");
        printf("]\n");
        return 10;
    }
    printf("IMPOSSIBLE\n");
    return 20;
}
