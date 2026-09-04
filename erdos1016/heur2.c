/*
 * Erdos 1016: randomised local search for a pancyclic C_n + k chords.
 *
 * The exhaustive searcher in pancyclic.c proves lower bounds on h(n); this one
 * only looks for witnesses, but it looks much faster.  Scoring a chord set means
 * walking the 2^(k+1)-1 elements of the cycle space and counting how many of the
 * lengths 3..n they realise, which is a few thousand operations, so hill
 * climbing with restarts explores far more of the space than DFS can.
 *
 * Usage: ./heur <n> <k> [seconds] [seed]
 * Exit 10 if a witness was found (printed), 1 otherwise.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

static int N, K, NE;
static int eu[64], ev[64];
static uint64_t vmask[64];

static uint64_t rngstate;
static inline uint64_t rnd(void)
{
    rngstate ^= rngstate << 13; rngstate ^= rngstate >> 7; rngstate ^= rngstate << 17;
    return rngstate;
}

static int cyclen(uint64_t mask)
{
    int ne = __builtin_popcountll(mask);
    if (ne < 3) return 0;
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
    int start = eu[__builtin_ctzll(mask)];
    int prev = -1, cur = start, steps = 0;
    do {
        int nx = (nb[cur][0] == prev) ? nb[cur][1] : nb[cur][0];
        prev = cur; cur = nx; steps++;
        if (steps > ne) return 0;
    } while (cur != start);
    return (steps == ne) ? ne : 0;
}

/* number of lengths in 3..N realised by C_N plus the given chords */
static int score(const int *cu, const int *cv)
{
    uint64_t fund[16], elem[1 << 7];
    uint64_t H = 0;
    for (int i = 0; i < N; i++) H |= 1ULL << i;
    fund[0] = H;
    for (int j = 0; j < K; j++) {
        eu[N + j] = cu[j]; ev[N + j] = cv[j];
        vmask[N + j] = (1ULL << cu[j]) | (1ULL << cv[j]);
        uint64_t f = 1ULL << (N + j);
        for (int i = cu[j]; i < cv[j]; i++) f |= 1ULL << i;
        fund[j + 1] = f;
    }
    elem[0] = 0;
    uint64_t seen = 0;
    int cnt = 0, cycles = 0;
    for (int s = 1; s < (1 << (K + 1)); s++) {
        int b = __builtin_ctz(s);
        elem[s] = elem[s ^ (1 << b)] ^ fund[b];
        int L = cyclen(elem[s]);
        if (L) cycles++;
        if (L >= 3 && L <= N && !((seen >> L) & 1)) { seen |= 1ULL << L; cnt++; }
    }
    /* lexicographic: cover as many lengths as possible, and among ties prefer
       chord sets whose cycle space contains more actual cycles */
    return cnt * 128 + cycles;
}

int main(int argc, char **argv)
{
    N = atoi(argv[1]); K = atoi(argv[2]);
    double budget = argc > 3 ? atof(argv[3]) : 60.0;
    rngstate = argc > 4 ? (uint64_t)atoll(argv[4]) : 88172645463325252ULL;
    NE = N + K;
    for (int i = 0; i < N - 1; i++) { eu[i] = i; ev[i] = i + 1; }
    eu[N - 1] = 0; ev[N - 1] = N - 1;
    for (int i = 0; i < N; i++) vmask[i] = (1ULL << eu[i]) | (1ULL << ev[i]);

    int NCH = 0, chu[2048], chv[2048];
    for (int i = 0; i < N; i++)
        for (int j = i + 2; j < N; j++) {
            if (i == 0 && j == N - 1) continue;
            chu[NCH] = i; chv[NCH] = j; NCH++;
        }

    int target = (N - 2) * 128;
    int cu[16], cv[16], idx[16], bestscore = 0, bidx[16];
    struct timespec t0, t1; clock_gettime(CLOCK_MONOTONIC, &t0);
    long long evals = 0;

    for (;;) {
        for (int j = 0; j < K; j++) if (rnd() % 4 == 0 || bestscore == 0) {
            idx[j] = rnd() % NCH; cu[j] = chu[idx[j]]; cv[j] = chv[idx[j]];
        }
        int cur = score(cu, cv); evals++;
        for (int stall = 0; stall < 3 * K; ) {
            int j = rnd() % K, old = idx[j], improved = 0;
            /* try a batch of random replacements for chord j, keep the best */
            int bestc = old, bests = cur;
            for (int tries = 0; tries < 64; tries++) {
                int c = rnd() % NCH;
                idx[j] = c; cu[j] = chu[c]; cv[j] = chv[c];
                int s = score(cu, cv); evals++;
                if (s > bests) { bests = s; bestc = c; }
            }
            idx[j] = bestc; cu[j] = chu[bestc]; cv[j] = chv[bestc];
            if (bests > cur) { cur = bests; improved = 1; stall = 0; } else stall++;
            (void)improved;
            if (cur >= target) break;
        }
        if (cur > bestscore) { bestscore = cur; memcpy(bidx, idx, sizeof idx); }
        /* iterated local search: perturb rather than restart from scratch */
        if (cur >= target - 128 * 2) {
            for (int j = 0; j < K; j++) if (rnd() % 3 == 0) {
                int c = rnd() % NCH; idx[j] = c; cu[j] = chu[c]; cv[j] = chv[c];
            }
        }
        if (cur >= target) {
            printf("n=%d k=%d FOUND chords=[", N, K);
            for (int j = 0; j < K; j++) printf("(%d,%d)%s", cu[j], cv[j], j + 1 < K ? "," : "");
            printf("] evals=%lld\n", evals);
            return 10;
        }
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if ((t1.tv_sec - t0.tv_sec) + 1e-9 * (t1.tv_nsec - t0.tv_nsec) > budget) break;
    }
    printf("n=%d k=%d no witness found (best %d of %d lengths, %lld evaluations)\n",
           N, K, bestscore / 128, target / 128, evals);
    return 1;
}
