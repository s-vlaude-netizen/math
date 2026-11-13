#!/usr/bin/env python3
"""orbits.py N < solver "S ..." lines: the distinct chord sets with n == N up to the 2N symmetries of the cycle."""
import sys

N = int(sys.argv[1])
seen = {}
for line in sys.stdin:
    t = line.split()
    if not t or t[0] != "S" or int(t[2]) != N:
        continue
    ch = [tuple(map(int, x.split(","))) for x in t[t.index("C") + 1:]]
    best = None
    for fl in (0, 1):
        for r in range(N):
            img = tuple(sorted(tuple(sorted((((N - u) % N if fl else u) + r) % N for u in e)) for e in ch))
            if best is None or img < best:
                best = img
    seen.setdefault(best, []).append(t[1])
for c, types in sorted(seen.items()):
    print(" ".join(f"({u},{v})" for u, v in c), "  types", ",".join(sorted(set(types))), " arc vectors", len(types))
print(f"# n={N}: {len(seen)} graphs up to symmetry, from {sum(len(v) for v in seen.values())} arc vectors")
