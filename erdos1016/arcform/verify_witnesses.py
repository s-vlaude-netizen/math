#!/usr/bin/env python3
"""
verify_witnesses.py K FROM TO OUT.json < solver "S ..." lines (one per n, chords after "C")

Independent check of witnesses: builds the cycle on n vertices plus the chords and enumerates ALL
simple cycles with networkx; requires exactly K distinct chords (no cycle edges) and a cycle of
every length 3..n.  Writes the verified list as JSON.
"""
import json
import sys

import networkx as nx

K, lo, hi, out = int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]), sys.argv[4]
res = {}
for line in sys.stdin:
    t = line.split()
    if not t or t[0] != "S":
        continue
    n = int(t[2])
    ch = sorted(tuple(sorted(map(int, x.split(",")))) for x in t[t.index("C") + 1:])
    assert len(set(ch)) == K, (n, ch)
    assert all(0 <= u < v < n and v - u not in (1, n - 1) for u, v in ch), (n, ch)
    g = nx.Graph()
    g.add_nodes_from(range(n))
    for i in range(n):
        g.add_edge(i, (i + 1) % n)
    g.add_edges_from(ch)
    assert g.number_of_edges() == n + K
    lens = {len(c) for c in nx.simple_cycles(g)}
    assert lens == set(range(3, n + 1)), f"n={n}: not pancyclic, missing {sorted(set(range(3, n + 1)) - lens)}"
    res[n] = dict(n=n, edges=n + K, chords=ch)
assert sorted(res) == list(range(lo, hi + 1)), f"missing n: {sorted(set(range(lo, hi + 1)) - set(res))}"
json.dump([res[n] for n in sorted(res)], open(out, "w"), indent=0)
print(f"{len(res)} witnesses with {K} chords, n = {lo}..{hi}: all pancyclic (networkx, all simple cycles)")
