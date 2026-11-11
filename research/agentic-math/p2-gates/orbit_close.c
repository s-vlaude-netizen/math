/*
 * orbit_close.c -- read chord sets ("u,v u,v ..." per line) of C_N and print every image
 * under the 2N symmetries x -> +-x + t (mod N), chords normalised (u<v) and sorted.
 * Piped through `sort -u`, the output of a symmetry-reduced search must equal the output
 * of the naive reference exactly.
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 2) { fprintf(stderr, "usage: %s N < sets\n", argv[0]); return 1; }
    int N = atoi(argv[1]);
    char line[4096];
    while (fgets(line, sizeof line, stdin)) {
        int u[32], v[32], k = 0; char *p = line;
        while (*p) {
            while (*p == ' ') p++;
            if (*p < '0' || *p > '9') break;
            u[k] = (int)strtol(p, &p, 10); if (*p == ',') p++;
            v[k] = (int)strtol(p, &p, 10); k++;
            if (k >= 32) break;
        }
        for (int g = 0; g < 2 * N; g++) {
            int t = g % N, fl = g >= N, key[32];
            for (int i = 0; i < k; i++) {
                int a = u[i], b = v[i];
                if (fl) { a = (N - a) % N; b = (N - b) % N; }
                a = (a + t) % N; b = (b + t) % N;
                if (a > b) { int x = a; a = b; b = x; }
                key[i] = a * 64 + b;
            }
            for (int i = 1; i < k; i++) { int x = key[i], q = i - 1; while (q >= 0 && key[q] > x) { key[q + 1] = key[q]; q--; } key[q + 1] = x; }
            for (int i = 0; i < k; i++) printf("%d,%d%s", key[i] / 64, key[i] % 64, i + 1 < k ? " " : "");
            printf("\n");
        }
    }
    return 0;
}
