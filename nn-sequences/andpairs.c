/* andpairs.c N : the Boolean functions of N variables that are the AND of two threshold functions.
   A = { h1 & h2 : h1, h2 threshold functions }.  What a network with two hidden threshold neurons and a
   threshold output neuron computes is A together with the complements of A (the output neuron is a
   threshold function of two bits, i.e. AND or OR of possibly negated hidden outputs, and the set of
   threshold functions is closed under negation).
   Prints  N  #threshold  |A|  |A union not-A|  |A intersect not-A| .  Reads thr_N.u32 (uint32 truth tables). */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    char fn[64]; snprintf(fn, sizeof fn, "thr_%d.u32", n);
    FILE *f = fopen(fn, "rb"); if (!f) { perror(fn); return 1; }
    fseek(f, 0, SEEK_END); long m = ftell(f) / 4; fseek(f, 0, SEEK_SET);
    uint32_t *H = malloc(m * 4); if (fread(H, 4, m, f) != (size_t)m) return 1; fclose(f);
    int bits = 1 << n; uint64_t total = 1ULL << bits;            /* number of functions */
    uint32_t mask = bits == 32 ? 0xFFFFFFFFu : (1u << bits) - 1;
    uint8_t *bs = calloc(total / 8 + 1, 1);
    for (long i = 0; i < m; i++) {
        uint32_t a = H[i];
        for (long j = i; j < m; j++) { uint32_t v = a & H[j]; bs[v >> 3] |= 1u << (v & 7); }
        if ((i & 8191) == 0) { fprintf(stderr, "%ld/%ld\r", i, m); }
    }
    if (argc > 2) { FILE *o = fopen(argv[2], "wb"); fwrite(bs, 1, total / 8, o); fclose(o); }
    uint64_t cA = 0, cU = 0, cI = 0;
    for (uint64_t v = 0; v < total; v++) {
        int a = bs[v >> 3] >> (v & 7) & 1;
        uint32_t c = (~(uint32_t)v) & mask;
        int b = bs[c >> 3] >> (c & 7) & 1;
        cA += a; cU += (a | b); cI += (a & b);
    }
    printf("%d %ld %llu %llu %llu\n", n, m, (unsigned long long)cA, (unsigned long long)cU, (unsigned long long)cI);
    return 0;
}
