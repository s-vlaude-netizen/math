/* unions.c N : labeled graphs on N vertices that are the edge-union of two threshold graphs (threshold dimension <= 2),
   and the number of isomorphism classes of them (Burnside over cycle types of S_N).
   Reads thr_N.u32 (all labeled threshold graphs as bitmasks over the C(N,2) pairs, lexicographic order).
   Self-tests of the Burnside code: the threshold graphs themselves must give 2^(N-1) classes; for N <= 7 the set of all
   graphs must give A000088 (1, 2, 4, 11, 34, 156, 1044). */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
static int N, NE, eidx[8][8];
static uint32_t tab[4][256];
static int part[16], np;
static double fact(int n) { double f = 1; for (int i = 2; i <= n; i++) f *= i; return f; }
static uint64_t count_bits(const uint8_t *bs, uint64_t total) { uint64_t c = 0; for (uint64_t i = 0; i < (total + 7) / 8; i++) c += __builtin_popcount(bs[i]); return c; }
static void set_perm(const int *sigma) {
    memset(tab, 0, sizeof tab);
    for (int b = 0; b < 4; b++) for (int x = 0; x < 256; x++) {
        uint32_t im = 0;
        for (int k = 0; k < 8; k++) if (x >> k & 1) {
            int e = b * 8 + k; if (e >= NE) continue;
            int i = 0, j = 0, c = 0;
            for (i = 0; i < N; i++) { for (j = i + 1; j < N; j++) { if (c == e) goto found; c++; } }
            found:;
            int a = sigma[i], d = sigma[j]; if (a > d) { int t = a; a = d; d = t; }
            im |= 1u << eidx[a][d];
        }
        tab[b][x] = im;
    }
}
static double burnside(const uint8_t *bs, uint64_t total) {
    double sum = 0; int sigma[8];
    /* all partitions of N as cycle types */
    int cnt[9]; memset(cnt, 0, sizeof cnt);
    int stack[16]; int sp = 0;
    /* iterate partitions via non-increasing sequences */
    int a[16]; int k = 0; a[0] = N; k = 1;
    for (;;) {
        /* build permutation with cycle lengths a[0..k-1] */
        int pos = 0; double denom = 1; int m[9]; memset(m, 0, sizeof m);
        for (int t = 0; t < k; t++) { for (int u = 0; u < a[t]; u++) sigma[pos + u] = pos + (u + 1) % a[t]; pos += a[t]; m[a[t]]++; }
        for (int l = 1; l <= N; l++) { for (int q = 0; q < m[l]; q++) denom *= l; denom *= fact(m[l]); }
        set_perm(sigma);
        uint64_t fix = 0;
        for (uint64_t g = 0; g < total; g++) if (bs[g >> 3] >> (g & 7) & 1) {
            uint32_t im = tab[0][g & 255] | tab[1][(g >> 8) & 255] | tab[2][(g >> 16) & 255] | tab[3][(g >> 24) & 255];
            fix += (im == (uint32_t)g);
        }
        sum += (fact(N) / denom) * (double)fix;
        /* next partition in reverse lexicographic order */
        int rem = 0, t = k - 1;
        while (t >= 0 && a[t] == 1) { rem++; t--; }
        if (t < 0) break;
        a[t]--; rem++; k = t + 1;
        int lim = a[t];
        while (rem > 0) { int p = rem < lim ? rem : lim; a[k++] = p; rem -= p; }
    }
    (void)cnt; (void)stack; (void)sp;
    return sum / fact(N);
}
int main(int argc, char **argv) {
    N = atoi(argv[1]); NE = N * (N - 1) / 2;
    for (int i = 0, c = 0; i < N; i++) for (int j = i + 1; j < N; j++) { eidx[i][j] = c; eidx[j][i] = c; c++; }
    char fn[64]; snprintf(fn, sizeof fn, "thr_%d.u32", N);
    FILE *f = fopen(fn, "rb"); fseek(f, 0, SEEK_END); long m = ftell(f) / 4; fseek(f, 0, SEEK_SET);
    uint32_t *T = malloc(m * 4); if (fread(T, 4, m, f) != (size_t)m) return 1; fclose(f);
    uint64_t total = 1ULL << NE;
    uint8_t *b1 = calloc(total / 8 + 1, 1), *b2 = calloc(total / 8 + 1, 1);
    for (long i = 0; i < m; i++) b1[T[i] >> 3] |= 1u << (T[i] & 7);
    for (long i = 0; i < m; i++) { uint32_t a = T[i]; for (long j = i; j < m; j++) { uint32_t v = a | T[j]; b2[v >> 3] |= 1u << (v & 7); } }
    uint64_t lab = count_bits(b2, total);
    double ot = burnside(b1, total), ou = burnside(b2, total);
    printf("N=%d threshold=%ld (unlabeled %.0f, expect %d) | unions: labeled %llu, unlabeled %.0f", N, m, ot, 1 << (N - 1), (unsigned long long)lab, ou);
    if (N <= 7) { uint8_t *all = malloc(total / 8 + 1); memset(all, 0xFF, total / 8 + 1); if (total < 8) all[0] = (1 << total) - 1; printf(" | all graphs unlabeled %.0f", burnside(all, total)); }
    printf("\n"); return 0;
}
