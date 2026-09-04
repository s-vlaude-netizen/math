/*
 * Erdos Problem #1016  --  https://www.erdosproblems.com/1016
 *
 * h(n) = minimal h such that some graph on n vertices with n+h edges is
 * pancyclic (contains a cycle of every length k, 3 <= k <= n).
 * OEIS A105206 lists n + h(n).
 *
 * Two observations make an exhaustive search possible:
 *
 *  (1) A pancyclic graph contains an n-cycle, so it is Hamiltonian.  Relabel
 *      so that the Hamiltonian cycle is 0-1-...-(n-1)-0; the graph is then
 *      C_n plus k = h chords.
 *
 *  (2) Its cycle space has dimension m - n + 1 = k + 1, so the graph has at
 *      most 2^(k+1) - 1 cycles in total.  Every cycle is the XOR of a subset
 *      of the k+1 fundamental cycles, so the whole cycle spectrum can be read
 *      off from at most 2^(k+1) - 1 bitmasks.
 *      (This also gives Bondy's lower bound 2^(k+1) - 1 >= n - 2.)
 *
 * The search is a DFS over chord sets that computes the cycle spectrum
 * incrementally and prunes with the counting bound of (2).
 *
 * Usage: ./pancyclic <n> <k> [firstchord_index]
 * Exit 10 = pancyclic C_n + k chords found (printed), 20 = proved impossible.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

static int N, K;
static int NCH;                 /* number of chords          */
static int chu[2048], chv[2048];/* chord endpoints           */
static uint64_t fund[16];       /* fundamental cycle masks: fund[0]=Hamilton, fund[j+1]=chord j */
static uint64_t elem[1 << 6];   /* cycle-space elements, indexed by subset  */
static int elen[1 << 6];        /* length if the element is a single cycle, else 0 */
static int eu[64], ev[64];      /* endpoints of edge index   */
static uint64_t vmask[64];      /* (1<<u)|(1<<v) per edge    */
static int NE;                  /* total edges = N + K       */
static int chosen[16];
static long long nodes = 0;
static int lensCount[64];       /* how many chosen elements realise each length */
static int distinctLens;        /* number of lengths in 3..N realised          */
static int best[16], haveSol = 0;
static int FIRST = -1;
static int CANON = 0;    /* 1 = restrict to canonical chord sets (see below) */
static int SLICEI = -1, SLICEM = 0;  /* split the depth-1 branching over processes */
static int MINSPAN = 0;  /* the forced minimum span s; chord (0,s) is always present */
static int cand[2048], NCAND;   /* chords with span >= MINSPAN, excluding (0,MINSPAN) */

/* span of a chord = distance along the Hamiltonian cycle */
static int span(int u, int v) { int d = v - u; return d < N - d ? d : N - d; }

/* is the edge set a single cycle?  returns its length, or 0 */
static int cyclen(uint64_t mask)
{
    int ne = __builtin_popcountll(mask);
    if (ne < 3) return 0;
    /* Cheap rejection first: d1 = vertices of degree >= 1, d2 = degree >= 2.
       A 2-regular edge set is exactly one with d1 == d2 and no vertex over 2.
       Almost every cycle-space element dies here, so the expensive adjacency
       walk below runs only for the rare survivors. */
    uint64_t d1 = 0, d2 = 0, t = mask;
    while (t) {
        int e = __builtin_ctzll(t); t &= t - 1;
        uint64_t vm = vmask[e];
        if (vm & d2) return 0;
        d2 |= vm & d1;
        d1 |= vm;
    }
    if (d1 != d2) return 0;

    int deg[64], nb[64][2];
    uint64_t vv = d1;
    while (vv) { int v = __builtin_ctzll(vv); vv &= vv - 1; deg[v] = 0; }
    t = mask;
    while (t) {
        int e = __builtin_ctzll(t); t &= t - 1;
        int a = eu[e], b = ev[e];
        nb[a][deg[a]++] = b; nb[b][deg[b]++] = a;
    }
    /* connected?  a 2-regular set is a disjoint union of cycles */
    int start = eu[__builtin_ctzll(mask)];
    int prev = -1, cur = start, steps = 0;
    do {
        int nx = (nb[cur][0] == prev) ? nb[cur][1] : nb[cur][0];
        prev = cur; cur = nx; steps++;
        if (steps > ne) return 0;
    } while (cur != start);
    return (steps == ne) ? ne : 0;
}

static void addLen(int L) { if (L >= 3 && L <= N && lensCount[L]++ == 0) distinctLens++; }
static void delLen(int L) { if (L >= 3 && L <= N && --lensCount[L] == 0) distinctLens--; }

/* DFS: j chords already chosen (indices strictly increasing, >= start) */
static int dfs(int j, int start)
{
    nodes++;
    if (distinctLens == N - 2) {          /* every length 3..N present */
        memcpy(best, chosen, sizeof(int) * K); haveSol = 1; return 1;
    }
    if (j == K) return 0;
    /* counting prune: the remaining K-j chords can add at most
       2^(K+1) - 2^(j+1) further cycles, hence that many further lengths */
    int missing = (N - 2) - distinctLens;
    if (missing > (1 << (K + 1)) - (1 << (j + 1))) return 0;

    int limit = CANON ? NCAND : NCH;
    for (int ci = start; ci < limit; ci++) {
        if (j == 1 && SLICEM && ci % SLICEM != SLICEI) continue;
        int c = CANON ? cand[ci] : ci;
        if (j == 0 && FIRST >= 0 && ci != FIRST) continue;
        chosen[j] = c;
        int ei = N + j;                    /* edge index of this chord */
        eu[ei] = chu[c]; ev[ei] = chv[c];
        vmask[ei] = (1ULL << chu[c]) | (1ULL << chv[c]);
        /* fundamental cycle: path edges [u, v) plus the chord itself */
        uint64_t f = 0;
        for (int i = chu[c]; i < chv[c]; i++) f |= 1ULL << i;
        f |= 1ULL << ei;
        fund[j + 1] = f;
        /* new cycle-space elements: those containing this chord */
        int lo = 1 << (j + 1), hi = 1 << (j + 2);
        int added[64], na = 0;
        for (int s = lo; s < hi; s++) {
            elem[s] = elem[s ^ lo] ^ f;
            /* The length of a cycle-space element, if it is a cycle at all, is
               just its edge count.  If that length is already realised by some
               shallower element, whether this one is a cycle cannot change
               distinctLens, so skip the expensive 2-regularity + connectivity
               test.  Backtracking is LIFO, so the shallower contributor always
               outlives this one and the bookkeeping stays consistent. */
            int ne = __builtin_popcountll(elem[s]);
            if (ne < 3 || ne > N || lensCount[ne] > 0) { elen[s] = 0; continue; }
            int L = cyclen(elem[s]);
            elen[s] = L;
            if (L) { addLen(L); added[na++] = L; }
        }
        if (dfs(j + 1, ci + 1)) return 1;
        for (int i = 0; i < na; i++) delLen(added[i]);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: %s n k [firstchord]\n", argv[0]); return 1; }
    N = atoi(argv[1]); K = atoi(argv[2]);
    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i], "--canon")) CANON = 1;
        else if (!strcmp(argv[i], "--span") && i + 1 < argc) { CANON = 1; MINSPAN = atoi(argv[++i]); }
        else if (!strcmp(argv[i], "--slice") && i + 2 < argc) { SLICEI = atoi(argv[++i]); SLICEM = atoi(argv[++i]); }
        else FIRST = atoi(argv[i]);
    }
    NE = N + K;
    if (NE > 62) { fprintf(stderr, "too many edges\n"); return 1; }

    /* Hamiltonian cycle edges: i -> i+1 for i < N-1, and edge N-1 closing (N-1,0) */
    for (int i = 0; i < N - 1; i++) { eu[i] = i; ev[i] = i + 1; }
    eu[N - 1] = 0; ev[N - 1] = N - 1;
    for (int i = 0; i < N; i++) vmask[i] = (1ULL << eu[i]) | (1ULL << ev[i]);
    for (int i = 0, c = 0; i < N; i++)
        for (int j = i + 2; j < N; j++) {
            if (i == 0 && j == N - 1) continue;      /* that is a cycle edge */
            chu[c] = i; chv[c] = j; c++; NCH = c;
        }
    /* Hamiltonian cycle as a cycle-space element */
    uint64_t H = 0; for (int i = 0; i < N; i++) H |= 1ULL << i;
    fund[0] = H;
    elem[0] = 0; elen[0] = 0;
    elem[1] = H; elen[1] = N;
    memset(lensCount, 0, sizeof lensCount); distinctLens = 0;
    addLen(N);

    struct timespec t0, t1; clock_gettime(CLOCK_MONOTONIC, &t0);
    int r = 0;
    if (!CANON) {
        r = dfs(0, 0);
    } else if (K == 0) {
        r = (distinctLens == N - 2);          /* C_n by itself (only n = 3) */
    } else {
        /* Every chord set can be rotated so that a chord of minimum span s
           becomes (0,s); all other chords then have span >= s.  Looping over
           s = 2..floor(N/2) and forcing (0,s) therefore covers every set up to
           rotation, at roughly a factor-N saving. */
        for (int sp = 2; sp <= N / 2 && !r; sp++) {
            if (MINSPAN && sp != MINSPAN) continue;
            /* force chord (0,sp) as the first chord */
            int c0 = -1;
            for (int c = 0; c < NCH; c++) if (chu[c] == 0 && chv[c] == sp) c0 = c;
            if (c0 < 0) continue;
            NCAND = 0;
            for (int c = 0; c < NCH; c++)
                if (c != c0 && span(chu[c], chv[c]) >= sp) cand[NCAND++] = c;
            /* place (0,sp) by hand as chord 0, then recurse over the rest */
            chosen[0] = c0;
            eu[N] = 0; ev[N] = sp; vmask[N] = 1ULL | (1ULL << sp);
            uint64_t f = 0;
            for (int i = 0; i < sp; i++) f |= 1ULL << i;
            f |= 1ULL << N;
            fund[1] = f;
            int added[64], na = 0;
            for (int st = 2; st < 4; st++) {
                elem[st] = elem[st ^ 2] ^ f;
                int ne = __builtin_popcountll(elem[st]);
                if (ne < 3 || ne > N || lensCount[ne] > 0) { elen[st] = 0; continue; }
                int L = cyclen(elem[st]); elen[st] = L;
                if (L) { addLen(L); added[na++] = L; }
            }
            r = dfs(1, 0);
            if (!r) for (int i = 0; i < na; i++) delLen(added[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double secs = (t1.tv_sec - t0.tv_sec) + 1e-9 * (t1.tv_nsec - t0.tv_nsec);
    printf("n=%d k=%d nodes=%lld time=%.2fs ", N, K, nodes, secs);
    if (r) {
        printf("PANCYCLIC chords=[");
        for (int i = 0; i < K; i++) printf("(%d,%d)%s", chu[best[i]], chv[best[i]], i + 1 < K ? "," : "");
        printf("]\n"); return 10;
    }
    printf("IMPOSSIBLE\n"); return 20;
}
