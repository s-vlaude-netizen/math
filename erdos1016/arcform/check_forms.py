#!/usr/bin/env python3
"""
check_forms.py -- gate for the cycle forms of the types.

    ./gentypes K --part i m | python3 check_forms.py [REPEATS] [SEED]

For every type read from stdin and REPEATS random arc-length vectors, build the actual graph
(cycle on n vertices plus the chords) and enumerate ALL its simple cycles with networkx.  The set of
cycle lengths must equal { m_j + sum of the arcs of form j } plus the Hamiltonian length n, and the
number of simple cycles must be at least the number of forms + 1.
Shares no code with gentypes.c / arcform.py beyond the line format.
"""
import random
import sys

import networkx as nx

repeats = int(sys.argv[1]) if len(sys.argv) > 1 else 3
rng = random.Random(int(sys.argv[2]) if len(sys.argv) > 2 else 1)
ntypes = nchecks = 0
byp = {}
for line in sys.stdin:
    t = line.split()
    idx, p, F = int(t[0]), int(t[1]), int(t[2])
    lo = list(map(int, t[3:3 + p]))
    fl = list(map(int, t[3 + p:3 + p + 2 * F]))
    forms = [(fl[2 * i], fl[2 * i + 1]) for i in range(F)]
    e = t.index("E")
    ed = list(map(int, t[e + 1:]))
    edges = [(ed[i], ed[i + 1]) for i in range(0, len(ed), 2)]
    ntypes += 1
    byp[p] = byp.get(p, 0) + 1
    for _ in range(repeats):
        a = [lo[i] + rng.choice([0, 0, 0, 1, 2, 3, 5, 8]) for i in range(p)]
        n = sum(a)
        pos = [0]
        for x in a[:-1]:
            pos.append(pos[-1] + x)
        g = nx.Graph()
        for v in range(n):
            g.add_edge(v, (v + 1) % n)
        chords = {tuple(sorted((pos[i], pos[j]))) for i, j in edges}
        assert len(chords) == len(edges), f"type {idx}: chords coincide"
        for u, v in chords:
            assert not g.has_edge(u, v), f"type {idx}: chord {u},{v} is a cycle edge for arcs {a}"
            g.add_edge(u, v)
        cycles = [len(c) for c in nx.simple_cycles(g)]
        want = {m + sum(a[i] for i in range(p) if mask >> i & 1) for m, mask in forms} | {n}
        if set(cycles) != want or len(cycles) < F + 1:
            print(f"MISMATCH type {idx} arcs {a}: forms {sorted(want)} networkx {sorted(set(cycles))} ({len(cycles)} cycles, {F} forms)")
            sys.exit(1)
        nchecks += 1
print(f"{ntypes} types, {nchecks} random graphs: cycle lengths from the forms equal the lengths of all simple cycles; types by endpoints {dict(sorted(byp.items()))}")
