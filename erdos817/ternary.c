/* Least N such that {1..N} has an n-set A whose 3^n sums sum c_i a_i (c_i in {0,1,2}) are distinct
   (A399720, Erdős problem 817).  usage: ternary n H : all qualifying n-sets with max <= H, report the min max.
   DFS picks elements in decreasing order; sums kept as a bitset; adding x keeps distinctness iff
   the popcount triples (m, m<<x, m<<2x pairwise disjoint). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define W 160
typedef uint64_t bs[W];
static int n, best, cur[16], bestset[16], words;
static long long nodes;
static int add(const uint64_t *m, int x, uint64_t *out) {   /* out = m | m<<x | m<<2x, 1 if disjoint */
    uint64_t t1[W], t2[W];
    int q = x >> 6, r = x & 63, q2 = (2*x) >> 6, r2 = (2*x) & 63;
    for (int i = words - 1; i >= 0; i--) {
        uint64_t a = i - q >= 0 ? m[i-q] << r : 0; if (r && i - q - 1 >= 0) a |= m[i-q-1] >> (64 - r);
        uint64_t b = i - q2 >= 0 ? m[i-q2] << r2 : 0; if (r2 && i - q2 - 1 >= 0) b |= m[i-q2-1] >> (64 - r2);
        if ((a & m[i]) | (b & m[i]) | (a & b)) return 0;
        t1[i] = a; t2[i] = b;
    }
    for (int i = 0; i < words; i++) out[i] = m[i] | t1[i] | t2[i];
    return 1;
}
static void rec(const uint64_t *m, int k, int hi) {         /* choose k more elements, each < hi */
    nodes++;
    if (k == 0) { best = cur[0]; memcpy(bestset, cur, sizeof cur); return; }
    for (int x = hi - 1; x >= k; x--) {
        if (k == n && x >= best) continue;                  /* first element = max, must beat best */
        if (k == n) x = x;                                   
        if (k < n && cur[0] >= best) return;
        uint64_t nm[W];
        if (!add(m, x, nm)) continue;
        cur[n - k] = x;
        rec(nm, k - 1, x);
        if (k == n) { /* after a full subtree, best may have dropped */ }
    }
}
int main(int argc, char **argv) {
    n = atoi(argv[1]); int H = atoi(argv[2]);
    words = (2 * n * H) / 64 + 2; if (words > W) { fprintf(stderr, "H too large\n"); return 1; }
    best = H + 1;
    uint64_t m[W]; memset(m, 0, sizeof m); m[0] = 1;
    /* search candidate maxima upward: the first max that has a completion is the answer */
    int from = n, to = H;
    if (argc > 3) { from = to = atoi(argv[3]); }          /* only this maximum */
    for (int N = from; N <= to; N++) {
        best = N + 1;
        uint64_t nm[W];
        if (!add(m, N, nm)) continue;
        cur[0] = N;
        rec(nm, n - 1, N);
        if (best == N) {
            printf("a(%d) = %d  set:", n, N); for (int i = 0; i < n; i++) printf(" %d", bestset[i]);
            printf("  nodes=%lld\n", nodes); return 0;
        }
        if (N % 20 == 0) { fprintf(stderr, "N=%d excluded, nodes=%lld\n", N, nodes); }
    }
    printf("none with max in [%d,%d], nodes=%lld\n", from, to, nodes); return 0;
}
