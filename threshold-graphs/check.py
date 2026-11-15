#!/usr/bin/env python3
"""Independent checks for n <= 6: (1) threshold graphs via the forbidden induced subgraphs 2K2, C4, P4 equal the generated list;
(2) labeled union count by brute force; (3) number of isomorphism classes by canonical form over all n! relabelings."""
import sys, itertools
import numpy as np

def run(n):
    pairs = [(i, j) for i in range(n) for j in range(i + 1, n)]
    idx = {p: k for k, p in enumerate(pairs)}
    E = len(pairs)
    # forbidden induced subgraphs on 4 vertices: 2K2 (2 disjoint edges), C4, P4  -> as edge-sets on 4 labeled vertices
    def key4(edges): return frozenset(edges)
    forb = set()
    for perm in itertools.permutations(range(4)):
        for base in ([(0, 1), (2, 3)], [(0, 1), (1, 2), (2, 3), (3, 0)], [(0, 1), (1, 2), (2, 3)]):
            forb.add(frozenset(tuple(sorted((perm[a], perm[b]))) for a, b in base))
    quads = list(itertools.combinations(range(n), 4))
    def is_threshold(mask):
        for q in quads:
            sub = frozenset(p for p in itertools.combinations(q, 2) if mask >> idx[p] & 1)
            sub = frozenset((q.index(a), q.index(b)) for a, b in sub)
            if sub in forb: return False
        return True
    T = [m for m in range(1 << E) if is_threshold(m)] if n >= 4 else list(range(1 << E))
    gen = set(np.fromfile(f"thr_{n}.u32", dtype=np.uint32).tolist())
    ok1 = set(T) == gen
    U = {a | b for a in T for b in T}
    # canonical forms
    perms = []
    for s in itertools.permutations(range(n)):
        perms.append([idx[tuple(sorted((s[i], s[j])))] for (i, j) in pairs])
    def canon(m):
        return min(sum(((m >> k) & 1) << pm[k] for k in range(E)) for pm in perms)
    cls = len({canon(m) for m in U})
    print(f"n={n}: forbidden-subgraph threshold list equals generated list: {ok1} ({len(T)}); union labeled {len(U)}; classes {cls}")

for n in range(3, int(sys.argv[1]) + 1):
    run(n)
