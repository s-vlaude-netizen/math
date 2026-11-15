/* andverify.c N DUMP SAMPLES : independent check of the table "f is the AND of two threshold functions".
   For random f decide membership WITHOUT the table: f = h1 & h2 with threshold h1, h2 forces h1, h2 to contain f,
   so collect H_f = { h threshold : f is contained in h } and look for a pair in H_f whose AND is exactly f.
   Compare with the bit stored in DUMP (written by andpairs).  Samples: uniform random f, f = (member) with one bit flipped,
   and genuine members. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
static uint64_t s = 88172645463325252ULL;
static uint64_t rnd(void) { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return s; }
int main(int argc, char **argv) {
    int n = atoi(argv[1]); long samples = atol(argv[3]);
    char fn[64]; snprintf(fn, sizeof fn, "thr_%d.u32", n);
    FILE *f = fopen(fn, "rb"); fseek(f, 0, SEEK_END); long m = ftell(f) / 4; fseek(f, 0, SEEK_SET);
    uint32_t *H = malloc(m * 4); if (fread(H, 4, m, f) != (size_t)m) return 1; fclose(f);
    uint64_t total = 1ULL << (1 << n); uint32_t mask = (1 << n) == 32 ? 0xFFFFFFFFu : (uint32_t)(total - 1);
    uint8_t *bs = malloc(total / 8 + 1); f = fopen(argv[2], "rb"); if (fread(bs, 1, total / 8, f) != total / 8) return 1; fclose(f);
    uint32_t *cand = malloc(m * 4);
    long mism = 0, skipped = 0, inA[3] = {0, 0, 0}, done[3] = {0, 0, 0};
    for (long it = 0; it < samples * 3; it++) {
        int kind = it % 3; uint32_t g;
        if (kind == 0) g = (uint32_t)rnd() & mask;                                  /* uniform random */
        else { g = H[rnd() % m] & H[rnd() % m]; if (kind == 1) g ^= 1u << (rnd() % (1 << n)); }   /* member, or member with a bit flipped */
        long c = 0;
        for (long i = 0; i < m; i++) if ((g & ~H[i]) == 0) cand[c++] = H[i];
        if (c > 4000) { skipped++; continue; }
        int mine = 0;
        for (long i = 0; i < c && !mine; i++) for (long j = i; j < c; j++) if ((cand[i] & cand[j]) == g) { mine = 1; break; }
        int table = bs[g >> 3] >> (g & 7) & 1;
        done[kind]++; inA[kind] += mine;
        if (mine != table) { mism++; printf("MISMATCH f=%08x independent=%d table=%d\n", g, mine, table); }
    }
    printf("n=%d: uniform %ld samples (%ld in A, %.3f%%), flipped %ld (%ld in A), members %ld (%ld in A); skipped %ld; mismatches %ld\n",
           n, done[0], inA[0], 100.0 * inA[0] / done[0], done[1], inA[1], done[2], inA[2], skipped, mism);
    return mism != 0;
}
