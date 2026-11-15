#!/usr/bin/env python3
"""
gen.py N : all labeled threshold graphs on vertices 0..N-1 as bitmasks over the C(N,2) pairs (i<j, lexicographic order).
A threshold graph is built from one vertex by repeatedly adding an isolated or a dominating vertex.  Labeled: the vertex added
last can be any vertex, so  T_n = { relabel(G', v) + (isolated | all edges at v) : v in 0..n-1, G' in T_{n-1} }  (deduplicated).
The count must equal A000? 1, 2, 8, 46, 332, 2874, 29024, 334982 (A005840, n = 1..8).  Writes thr_N.u32.
"""
import sys
import numpy as np

def edges(n):
    return {(i, j): k for k, (i, j) in enumerate((i, j) for i in range(n) for j in range(i + 1, n))}

def build(N):
    T = {1: {0}}
    for n in range(2, N + 1):
        e_old, e_new = edges(n - 1), edges(n)
        out = set()
        for v in range(n):
            mp = {k: e_new[tuple(sorted((a + (a >= v), b + (b >= v))))] for (a, b), k in e_old.items()}
            dom = sum(1 << e_new[tuple(sorted((v, u)))] for u in range(n) if u != v)
            for g in T[n - 1]:
                m = 0
                for k, t in mp.items():
                    if g >> k & 1: m |= 1 << t
                out.add(m); out.add(m | dom)
        T[n] = out
    return T

if __name__ == "__main__":
    N = int(sys.argv[1])
    T = build(N)
    known = {1: 1, 2: 2, 3: 8, 4: 46, 5: 332, 6: 2874, 7: 29024, 8: 334982}
    for n in range(1, N + 1):
        print(n, len(T[n]), "A005840:", known[n], "OK" if len(T[n]) == known[n] else "MISMATCH")
        np.array(sorted(T[n]), dtype=np.uint32).tofile(f"thr_{n}.u32")
