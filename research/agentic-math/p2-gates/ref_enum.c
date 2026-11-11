/*
 * ref_enum.c -- reference implementation for the gates.  Deliberately naive and
 * deliberately different from pcsearch.c: no cycle space, no pruning, no symmetry.
 *
 *   ref_enum N K          print every K-subset of chords of C_N that is pancyclic
 *   ref_enum --spectrum   read lines "n u,v u,v ...", print the cycle lengths of C_n + chords
 *
 * Cycle lengths come from a depth-first enumeration of simple cycles: each cycle is
 * found from its smallest vertex s by walking only through vertices larger than s.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int n;
static uint64_t adj[64], lens;

static void walk(int s, int v, uint64_t vis, int depth)
{
    uint64_t nb = adj[v];
    while (nb) {
        int w = __builtin_ctzll(nb); nb &= nb - 1;
        if (w == s) { if (depth >= 3) lens |= 1ULL << depth; }
        else if (w > s && !((vis >> w) & 1)) walk(s, w, vis | (1ULL << w), depth + 1);
    }
}

static uint64_t spectrum(void)
{
    lens = 0;
    for (int s = 0; s < n; s++) walk(s, s, 1ULL << s, 1);
    return lens;
}

static void base_cycle(void)
{
    for (int i = 0; i < n; i++) adj[i] = 0;
    for (int i = 0; i < n; i++) { int j = (i + 1) % n; adj[i] |= 1ULL << j; adj[j] |= 1ULL << i; }
}

static int K, nch, cu[4096], cv[4096], pick[16];
static uint64_t full;
static long long count = 0;

static void rec(int depth, int start)
{
    if (depth == K) {
        if ((spectrum() & full) == full) {
            for (int i = 0; i < K; i++) printf("%d,%d%s", cu[pick[i]], cv[pick[i]], i + 1 < K ? " " : "");
            printf("\n");
            count++;
        }
        return;
    }
    for (int c = start; c < nch; c++) {
        pick[depth] = c;
        adj[cu[c]] |= 1ULL << cv[c]; adj[cv[c]] |= 1ULL << cu[c];
        rec(depth + 1, c + 1);
        adj[cu[c]] &= ~(1ULL << cv[c]); adj[cv[c]] &= ~(1ULL << cu[c]);
    }
}

int main(int argc, char **argv)
{
    if (argc >= 2 && !strcmp(argv[1], "--spectrum")) {
        char line[4096];
        while (fgets(line, sizeof line, stdin)) {
            char *p = line; n = (int)strtol(p, &p, 10);
            if (n < 3 || n > 60) continue;
            base_cycle();
            while (*p) {
                while (*p == ' ') p++;
                if (*p < '0' || *p > '9') break;
                int u = (int)strtol(p, &p, 10); if (*p == ',') p++;
                int v = (int)strtol(p, &p, 10);
                if (u != v && u >= 0 && v >= 0 && u < n && v < n) { adj[u] |= 1ULL << v; adj[v] |= 1ULL << u; }
            }
            uint64_t m = spectrum(); int first = 1;
            for (int L = 3; L <= n; L++) if ((m >> L) & 1) { printf("%s%d", first ? "" : " ", L); first = 0; }
            printf("\n");
        }
        return 0;
    }
    if (argc < 3) { fprintf(stderr, "usage: %s N K | --spectrum\n", argv[0]); return 1; }
    n = atoi(argv[1]); K = atoi(argv[2]);
    base_cycle();
    for (int i = 0; i < n; i++)
        for (int j = i + 2; j < n; j++) {
            if (i == 0 && j == n - 1) continue;
            cu[nch] = i; cv[nch] = j; nch++;
        }
    full = 0; for (int L = 3; L <= n; L++) full |= 1ULL << L;
    rec(0, 0);
    fprintf(stderr, "# n=%d k=%d pancyclic sets=%lld\n", n, K, count);
    return 0;
}
