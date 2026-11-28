/* Knuth-style estimate of the number of expand() calls that clique5 makes for "clique5 n lb" (unsplit): same graph, vertex order,
   greedy colouring and pruning.  At every node one explored child with a non-empty candidate set is chosen uniformly at random and
   the branching factor b is multiplied in (unbiased); at depth 0 every root is sampled separately (stratified), K probes per root,
   K0 probes for the first 50 roots (the expensive ones).
   usage: est5 n lb K [K0 [seed]]   prints the estimate, a standard error, and where the cost sits (by root index). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
typedef uint64_t u64;
static int n, V, W, best;
static u64 *adj;
static int *ordpool, *colpool;
static u64 rs[2];
static inline u64 rnd(void) { u64 s1 = rs[0]; const u64 s0 = rs[1]; rs[0] = s0; s1 ^= s1 << 23; rs[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); return rs[1] + s0; }
static int isap(unsigned m) {
    int c = __builtin_popcount(m); if (c <= 2) return 1;
    int a = __builtin_ctz(m); unsigned r = m & (m - 1); int b = __builtin_ctz(r); int d = b - a;
    int cnt = 0; for (int p = a; p < n && ((m >> p) & 1); p += d) cnt++; return cnt == c;
}
static int colour(const u64 *P, int *order, int *col) {
    int cnt = 0, k = 0, nonempty = 1; u64 U[W], Q[W]; memcpy(U, P, W * 8);
    while (nonempty) {
        k++; memcpy(Q, U, W * 8); nonempty = 0;
        for (int w = 0; w < W; w++) while (Q[w]) {
            int b = __builtin_ctzll(Q[w]); int v = w * 64 + b;
            U[w] &= ~(1ULL << b); Q[w] &= ~(1ULL << b);
            u64 *a = adj + (size_t)v * W;
            for (int j = w; j < W; j++) Q[j] &= ~a[j];
            order[cnt] = v; col[cnt] = k; cnt++;
        }
        for (int w = 0; w < W; w++) if (U[w]) { nonempty = 1; break; }
    }
    return cnt;
}
/* estimate of the number of expand() calls in the subtree of a call with candidate set P at this depth, including the call itself */
static double probe(const u64 *P, int depth) {
    int *order = ordpool + (size_t)depth * V, *col = colpool + (size_t)depth * V;
    int cnt = colour(P, order, col);
    u64 Pc[W], chosen[W], NP[W]; memcpy(Pc, P, W * 8);
    int b = 0;
    for (int i = cnt - 1; i >= 0; i--) {
        if (depth + col[i] <= best) break;
        int v = order[i]; u64 *a = adj + (size_t)v * W; int any = 0;
        for (int w = 0; w < W; w++) { NP[w] = Pc[w] & a[w]; if (NP[w]) any = 1; }
        if (any) { b++; if (rnd() % (u64)b == 0) memcpy(chosen, NP, W * 8); }
        Pc[v >> 6] &= ~(1ULL << (v & 63));
    }
    if (b == 0) return 1.0;
    return 1.0 + (double)b * probe(chosen, depth + 1);
}
int main(int argc, char **argv) {
    n = atoi(argv[1]); int lb = atoi(argv[2]); int K = atoi(argv[3]); int K0 = argc > 4 ? atoi(argv[4]) : K;
    u64 seed = argc > 5 ? strtoull(argv[5], 0, 10) : 12345; rs[0] = seed * 0x9E3779B97F4A7C15ULL + 1; rs[1] = seed ^ 0xD1B54A32D192ED03ULL; for (int i = 0; i < 20; i++) rnd();
    V = (1 << n) - 1; W = (V + 63) / 64; best = lb - 1;
    ordpool = malloc((size_t)256 * V * sizeof(int)); colpool = malloc((size_t)256 * V * sizeof(int)); adj = calloc((size_t)V * W, 8);
    for (int a = 0; a < V; a++) for (int b = 0; b < V; b++) if (a != b) { unsigned m = (a + 1) & (b + 1); if (m && isap(m)) adj[(size_t)a * W + (b >> 6)] |= 1ULL << (b & 63); }
    u64 P[W]; memset(P, 0, W * 8); for (int i = 0; i < V; i++) P[i >> 6] |= 1ULL << (i & 63);
    int *order = ordpool, *col = colpool; int cnt = colour(P, order, col);
    u64 Pc[W], NP[W]; memcpy(Pc, P, W * 8);
    double total = 1.0, varsum = 0.0; int roots = 0; double bucket[5] = {0, 0, 0, 0, 0}; const int edge[4] = {10, 100, 1000, 10000};
    for (int i = cnt - 1; i >= 0; i--) {
        int ri = cnt - 1 - i;
        if (col[i] <= best) break;
        int v = order[i]; u64 *a = adj + (size_t)v * W; int any = 0;
        for (int w = 0; w < W; w++) { NP[w] = Pc[w] & a[w]; if (NP[w]) any = 1; }
        if (any) {
            int k = ri < 50 ? K0 : K; double s1 = 0, s2 = 0;
            for (int t = 0; t < k; t++) { double e = probe(NP, 1); s1 += e; s2 += e * e; }
            double mean = s1 / k, var = k > 1 ? (s2 / k - mean * mean) * k / (k - 1) : 0; if (var < 0) var = 0;
            total += mean; varsum += var / k; roots++;
            int bk = 0; while (bk < 4 && ri >= edge[bk]) bk++; bucket[bk] += mean;
        }
        Pc[v >> 6] &= ~(1ULL << (v & 63));
    }
    printf("n=%d lb=%d: roots explored %d of %d vertices; estimated expand() calls = %.4g (standard error %.2g, %.0f%%)\n", n, lb, roots, V, total, sqrt(varsum), 100 * sqrt(varsum) / total);
    printf("share of the estimated cost by root index: <10: %.1f%%  10-99: %.1f%%  100-999: %.1f%%  1000-9999: %.1f%%  >=10000: %.1f%%\n",
           100 * bucket[0] / total, 100 * bucket[1] / total, 100 * bucket[2] / total, 100 * bucket[3] / total, 100 * bucket[4] / total);
    return 0;
}
