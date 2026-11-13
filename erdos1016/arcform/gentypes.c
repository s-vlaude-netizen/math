/*
 * gentypes.c -- the types of cycle-plus-K-chords graphs, with their cycle forms (fast version of
 * `arcform.py dump K`; same line format, same canonical representatives, different order/index).
 *
 * A type: p chord endpoints in cyclic order (positions 0..p-1), K distinct chords between positions,
 * every position used, up to the 2p rotations and reflections.  The representative printed is the
 * lexicographically smallest image (sorted list of pairs), as in arcform.py.
 *
 * Output, one type per line:   idx p F  lo_0..lo_{p-1}  (m_j mask_j) x F  E  (i j) x K
 *   lo_i  lower bound of arc i (2 if a chord joins its two ends, else 1)
 *   every cycle of the skeleton except the outer one, as (chords used, mask of arcs used), sorted, distinct
 *
 * Usage: gentypes K [--minF X] [--part i m] [--count]
 *   --minF X    print only types with at least X distinct cycle forms
 *   --part i m  print only the types whose running number (among those passing --minF) is i mod m
 *   --count     print statistics only
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXK 9
static int K, p;
static int cur[MAXK];                 /* chosen pairs, as codes i*32+j, increasing */
static long long idx, printed, passing, labeled;
static int MINF, PART_I = 0, PART_M = 1, COUNT;
static long long byp[2 * MAXK + 1], maxF, orbitsum;

static int cmp_int(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

static int stab;                      /* number of the 2p symmetries that fix the current pattern */

static int canonical(void)
{
    int img[MAXK];
    stab = 1;
    for (int fl = 0; fl < 2; fl++)
        for (int t = 0; t < p; t++) {
            if (!fl && !t) continue;
            for (int c = 0; c < K; c++) {
                int u = cur[c] >> 5, v = cur[c] & 31;
                if (fl) { u = (p - u) % p; v = (p - v) % p; }
                u = (u + t) % p; v = (v + t) % p;
                img[c] = u < v ? u * 32 + v : v * 32 + u;
            }
            qsort(img, K, sizeof(int), cmp_int);
            int c;
            for (c = 0; c < K; c++) {
                if (img[c] < cur[c]) return 0;
                if (img[c] > cur[c]) break;
            }
            if (c == K) stab++;
        }
    return 1;
}

static void emit(void)
{
    int E = p + K;
    int ea[32], eb[32];                               /* ends of arcs 0..p-1 and chords p..p+K-1 */
    unsigned inc[32], fund[MAXK + 1];
    for (int i = 0; i < p; i++) { ea[i] = i; eb[i] = (i + 1) % p; }
    for (int c = 0; c < K; c++) { ea[p + c] = cur[c] >> 5; eb[p + c] = cur[c] & 31; }
    memset(inc, 0, sizeof inc);
    for (int e = 0; e < E; e++) { inc[ea[e]] |= 1u << e; inc[eb[e]] |= 1u << e; }
    fund[0] = (1u << p) - 1;
    for (int c = 0; c < K; c++) {
        unsigned m = 1u << (p + c);
        for (int i = ea[p + c]; i < eb[p + c]; i++) m |= 1u << i;
        fund[c + 1] = m;
    }
    static unsigned xr[1 << (MAXK + 1)];
    static int forms[1 << (MAXK + 1)];
    int nf = 0;
    xr[0] = 0;
    for (int mask = 1; mask < (1 << (K + 1)); mask++) {
        int b = __builtin_ctz(mask);
        unsigned s = xr[mask & (mask - 1)] ^ fund[b];
        xr[mask] = s;
        if (mask < 2 || !s) continue;
        int ok = 1, start = -1;
        for (int v = 0; v < p && ok; v++) {
            int d = __builtin_popcount(s & inc[v]);
            if (d == 2) { if (start < 0) start = v; }
            else if (d != 0) ok = 0;
        }
        if (!ok || start < 0) continue;
        int steps = 0, at = start, prev = -1;
        do {
            unsigned cand = s & inc[at];
            int e = __builtin_ctz(cand);
            if (e == prev) e = __builtin_ctz(cand & (cand - 1));
            at = ea[e] + eb[e] - at; prev = e; steps++;
        } while (at != start);
        if (steps != __builtin_popcount(s)) continue;
        forms[nf++] = (__builtin_popcount(s >> p) << 24) | (int)(s & ((1u << p) - 1));
    }
    qsort(forms, nf, sizeof(int), cmp_int);
    int F = 0;
    for (int i = 0; i < nf; i++) if (i == 0 || forms[i] != forms[i - 1]) forms[F++] = forms[i];
    long long my = idx++;
    byp[p]++; if (F > maxF) maxF = F;
    if (F < MINF) return;
    long long run = passing++;
    if (run % PART_M != PART_I || COUNT) return;
    printed++;
    int lo[32];
    for (int i = 0; i < p; i++) lo[i] = 1;
    for (int c = 0; c < K; c++) {
        int i = cur[c] >> 5, j = cur[c] & 31;
        if (j == i + 1) lo[i] = 2;
        if (i == 0 && j == p - 1) lo[p - 1] = 2;
    }
    printf("%lld %d %d", my, p, F);
    for (int i = 0; i < p; i++) printf(" %d", lo[i]);
    for (int i = 0; i < F; i++) printf(" %d %d", forms[i] >> 24, forms[i] & 0xffffff);
    printf(" E");
    for (int c = 0; c < K; c++) printf(" %d %d", cur[c] >> 5, cur[c] & 31);
    printf("\n");
}

/* choose pairs in increasing order; `covered` = positions used so far */
static void rec(int depth, int from_i, int from_j, unsigned covered)
{
    if (depth == K) {
        if (covered != (1u << p) - 1) return;
        labeled++;
        if (canonical()) { orbitsum += 2 * p / stab; emit(); }
        return;
    }
    int r = K - depth;
    if (p - __builtin_popcount(covered) > 2 * r) return;
    for (int i = from_i; i < p; i++) {
        /* later pairs have first element >= i, so every position below i must be covered already */
        if ((covered & ((1u << i) - 1)) != (1u << i) - 1) break;
        for (int j = (i == from_i ? from_j : i + 1); j < p; j++) {
            cur[depth] = i * 32 + j;
            rec(depth + 1, i, j + 1, covered | 1u << i | 1u << j);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) { fprintf(stderr, "usage: gentypes K [--minF X] [--part i m] [--count]\n"); return 2; }
    K = atoi(argv[1]);
    if (K < 1 || K > MAXK) { fprintf(stderr, "need 1 <= K <= %d\n", MAXK); return 2; }
    for (int a = 2; a < argc; a++) {
        if (!strcmp(argv[a], "--minF") && a + 1 < argc) MINF = atoi(argv[++a]);
        else if (!strcmp(argv[a], "--part") && a + 2 < argc) { PART_I = atoi(argv[a + 1]); PART_M = atoi(argv[a + 2]); a += 2; }
        else if (!strcmp(argv[a], "--count")) COUNT = 1;
        else { fprintf(stderr, "unknown option %s\n", argv[a]); return 2; }
    }
    for (p = 2; p <= 2 * K; p++) {
        if (p * (p - 1) / 2 < K) continue;
        rec(0, 0, 1, 0);
    }
    /* every labelled pattern lies in the orbit of exactly one printed type */
    if (orbitsum != labeled) { fprintf(stderr, "orbit sizes sum to %lld, but there are %lld labelled patterns\n", orbitsum, labeled); return 3; }
    if (COUNT) {
        printf("K=%d: %lld types (%lld labelled chord patterns); with at least %d cycle forms: %lld; most cycle forms: %lld\nby number of endpoints:", K, idx, labeled, MINF, passing, maxF);
        for (int q = 2; q <= 2 * K; q++) if (byp[q]) printf(" %d:%lld", q, byp[q]);
        printf("\n");
    }
    return 0;
}
