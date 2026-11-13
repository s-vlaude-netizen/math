#!/usr/bin/env python3
"""Structure of pancyclic chord sets: orbits under the dihedral group, spans, crossings,
and the full cycle-space table (which of the 2^(k+1)-1 elements are cycles, and their lengths).
Independent of the C searcher: plain Python sets.

    python3 analyze_extremal.py N file_with_chord_sets [--brief]
"""
import sys
from itertools import combinations

def parse(line):
    return tuple(sorted(tuple(sorted(map(int, p.split(",")))) for p in line.split()))

def images(n, S):
    for fl in (0, 1):
        for t in range(n):
            yield tuple(sorted(tuple(sorted((((n - u) % n if fl else u) + t) % n for u in c)) for c in S))

def span(n, c):
    d = c[1] - c[0]; return min(d, n - d)

def crosses(a, b):
    (u, v), (x, y) = a, b
    return (u < x < v < y) or (x < u < y < v)

def cycle_table(n, S):
    """For every non-empty subset of {H} + chords: (number of chords, has H, length or 0)."""
    k = len(S)
    edges = [(i, i + 1) for i in range(n - 1)] + [(0, n - 1)] + list(S)
    fund = [frozenset(range(n))] + [frozenset(range(u, v)) | {n + j} for j, (u, v) in enumerate(S)]
    out = []
    for mask in range(1, 1 << (k + 1)):
        s = set()
        for b in range(k + 1):
            if mask >> b & 1: s ^= fund[b]
        deg, adj = {}, {}
        for e in s:
            a, b2 = edges[e]
            adj.setdefault(a, []).append(b2); adj.setdefault(b2, []).append(a)
        L = 0
        if s and all(len(v) == 2 for v in adj.values()):
            start = next(iter(adj)); prev, cur, steps = None, start, 0
            while True:
                nxt = adj[cur][0] if adj[cur][0] != prev else adj[cur][1]
                prev, cur = cur, nxt; steps += 1
                if cur == start: break
            if steps == len(s): L = steps
        out.append((bin(mask >> 1).count("1"), mask & 1, L))
    return out

def describe(n, S, brief=False):
    k = len(S)
    tab = cycle_table(n, S)
    cyc = [L for _, _, L in tab if L]
    distinct = sorted(set(cyc))
    dup = sorted(L for L in set(cyc) if cyc.count(L) > 1)
    cr = sum(crosses(a, b) for a, b in combinations(S, 2))
    shared = sum(len(set(a) & set(b)) > 0 for a, b in combinations(S, 2))
    by_c = {}
    for c, h, L in tab:
        by_c.setdefault(c, [0, 0]); by_c[c][1] += 1; by_c[c][0] += bool(L)
    line = (f"{' '.join(f'({u},{v})' for u, v in S)}  spans={sorted(span(n, c) for c in S)}  crossing pairs={cr}/{k*(k-1)//2}"
            f"  sharing an endpoint={shared}  cycles={len(cyc)}/{len(tab)}  distinct lengths={len(distinct)}  repeated={dup}")
    if not brief:
        line += "\n      cycles by number of chords used: " + ", ".join(f"{c}:{by_c[c][0]}/{by_c[c][1]}" for c in sorted(by_c))
    return line

if __name__ == "__main__":
    n = int(sys.argv[1]); brief = "--brief" in sys.argv
    sets = {parse(l) for l in open(sys.argv[2]) if l.strip()}
    orbits = {}
    for S in sets:
        rep = min(images(n, S))
        orbits.setdefault(rep, set()).add(S)
    print(f"n={n}: {len(sets)} chord sets in {len(orbits)} orbits (sizes {sorted(len(v) for v in orbits.values())})")
    for rep in sorted(orbits):
        print("  " + describe(n, rep, brief))
