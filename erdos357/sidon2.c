/*
 * Erdos Problem #357  --  https://www.erdosproblems.com/357
 *
 * Let 1 <= a_1, ..., a_k <= n be integers such that all "consecutive sums"
 *      sum_{u <= i <= v} a_i        (1 <= u <= v <= k)
 * are distinct.  f(n) = max such k with a_1 < ... < a_k (monotone case),
 * g(n) = max such k without the monotonicity assumption.
 *
 * OEIS A364132 = inverse of f  (smallest n admitting a monotone example of length n)
 * OEIS A364153 = inverse of g  (smallest n admitting an arbitrary example of length n)
 *
 * KEY REFORMULATION used here
 * ---------------------------
 * Put P_0 = 0, P_i = a_1 + ... + a_i.  A consecutive sum is P_v - P_{u-1}, so
 *
 *      all consecutive sums distinct  <=>  {P_0, ..., P_k} is a Sidon set
 *                                          (a Golomb ruler with k+1 marks).
 *
 * The side condition a_i <= n says every gap of that ruler is at most n.
 * Hence
 *      A364153(k) = min over all Golomb rulers with k+1 marks of the largest gap,
 *      A364132(k) = the same, restricted to rulers whose gaps increase.
 *
 * This program decides, for given k (=n gaps) and m, whether such a ruler exists.
 *
 * Usage:  ./sidon <k> <m> <mode> [first_gap]
 *           mode 0 = arbitrary order   (A364153)
 *           mode 1 = increasing gaps   (A364132)
 *           first_gap (optional) restricts the search to rulers whose first gap
 *           equals that value -- used to split the work over several processes.
 *
 * Exit code 10 = ruler found (prints it), 20 = proved impossible.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

static int K;            /* number of gaps  (ruler has K+1 marks) */
static int M;            /* maximal allowed gap                   */
static int MODE;         /* 0 arbitrary, 1 increasing             */
static int FIRSTGAP = 0; /* 0 = all                               */
static int NEED;         /* K(K+1)/2 distinct differences needed  */
static int MAXT;

static uint64_t *used;   /* bit d set  <=>  difference d already realised */
static int marks[80];
static int gaps[80];
static long long nodes = 0;
static int solution[80], have_solution = 0;

static inline int getbit(int t) { return (used[t >> 6] >> (t & 63)) & 1ULL; }
static inline void setbit(int t) { used[t >> 6] |= 1ULL << (t & 63); }
static inline void clrbit(int t) { used[t >> 6] &= ~(1ULL << (t & 63)); }

/* sum of the r largest still-unused values in (lo, M]; -1 if fewer than r exist */
static int maxsum(int r, int lo)
{
    int s = 0;
    for (int g = M; g > lo && r > 0; g--)
        if (!getbit(g)) { s += g; r--; }
    return r > 0 ? -1 : s;
}

/* smallest possible additional length: r distinct unused gaps > lo */
static int minsum(int r, int lo)
{
    int s = 0;
    for (int g = lo + 1; g <= M && r > 0; g++)
        if (!getbit(g)) { s += g; r--; }
    return r > 0 ? -1 : s;
}

static int dfs(int k, int L, int lastgap)
{
    nodes++;
    if (k == K) {
        memcpy(solution, gaps, sizeof(int) * K);
        have_solution = 1;
        return 1;
    }
    int r = K - k;
    int lo = MODE ? lastgap : 0;

    /* Prune 1: the K(K+1)/2 differences are distinct positive integers bounded
       by the total length T, hence T >= K(K+1)/2. */
    int ms = maxsum(r, lo);
    if (ms < 0 || L + ms < NEED) return 0;
    /* Prune 2 (increasing mode): the remaining gaps must fit at all. */
    if (MODE && minsum(r, lo) < 0) return 0;

    /* Reversal symmetry (arbitrary-order mode only): reversing a solution gives
       another one, so we may demand first gap < last gap.  Enforced both as a
       constraint on the last gap and as a look-ahead: if no available gap
       exceeds the first one, the branch cannot be completed. */
    if (!MODE && k >= 1) {
        int ok = 0;
        for (int g = M; g > gaps[0]; g--) if (!getbit(g)) { ok = 1; break; }
        if (!ok) return 0;
    }
    for (int g = lo + 1; g <= M; g++) {
        if (getbit(g)) continue;
        if (k == 0 && FIRSTGAP && g != FIRSTGAP) continue;
        if (!MODE && k == K - 1 && k >= 1 && g <= gaps[0]) continue;
        int p = L + g;
        int dbuf[80], nd = 0, ok = 1;
        for (int i = k; i >= 0; i--) {
            int d = p - marks[i];
            if (getbit(d)) { ok = 0; break; }
            setbit(d);
            dbuf[nd++] = d;
        }
        if (ok) {
            marks[k + 1] = p;
            gaps[k] = g;
            if (dfs(k + 1, p, g)) { for (int i = 0; i < nd; i++) clrbit(dbuf[i]); return 1; }
        }
        for (int i = 0; i < nd; i++) clrbit(dbuf[i]);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 4) { fprintf(stderr, "usage: %s K M mode [first_gap]\n", argv[0]); return 1; }
    K = atoi(argv[1]); M = atoi(argv[2]); MODE = atoi(argv[3]);
    if (argc > 4) FIRSTGAP = atoi(argv[4]);
    NEED = K * (K + 1) / 2;
    MAXT = K * M + 2;
    used = calloc((MAXT >> 6) + 2, sizeof(uint64_t));
    marks[0] = 0;

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    int res = dfs(0, 0, 0);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double secs = (t1.tv_sec - t0.tv_sec) + 1e-9 * (t1.tv_nsec - t0.tv_nsec);

    printf("K=%d M=%d mode=%d firstgap=%d  nodes=%lld  time=%.2fs  ", K, M, MODE, FIRSTGAP, nodes, secs);
    if (res) {
        printf("FOUND gaps=[");
        for (int i = 0; i < K; i++) printf("%d%s", solution[i], i + 1 < K ? "," : "");
        printf("]\n");
        return 10;
    }
    printf("NONE\n");
    return 20;
}
