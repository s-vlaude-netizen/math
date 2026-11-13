#!/usr/bin/env python3
"""
arc2chords.py -- turn arcdfs solutions back into chord sets.

    ./arcdfs 0 --print < types_K.txt | python3 arc2chords.py types_K.txt [N]

Reads "S idx n a_0 .. a_{p-1}" lines; prints, for each (only those with n == N if N is given),
"n u,v u,v ..." (or just the chord set if N is given), with position 0 placed at vertex 0.
"""
import sys

types = {}
for line in open(sys.argv[1]):
    t = line.split()
    idx, p = int(t[0]), int(t[1])
    e = t.index("E")
    ed = list(map(int, t[e + 1:]))
    types[idx] = (p, [(ed[i], ed[i + 1]) for i in range(0, len(ed), 2)])
want = int(sys.argv[2]) if len(sys.argv) > 2 else None
for line in sys.stdin:
    if not line.startswith("S "):
        continue
    w = line.split()[1:]
    if "C" in w:                                   # arcdfs2 appends the chords itself; ignore them here
        w = w[:w.index("C")]
    t = list(map(int, w))
    idx, n, a = t[0], t[1], t[2:]
    if want is not None and n != want:
        continue
    p, edges = types[idx]
    assert len(a) == p and sum(a) == n
    pos = [0]
    for x in a[:-1]:
        pos.append(pos[-1] + x)
    cs = sorted(tuple(sorted((pos[i], pos[j]))) for i, j in edges)
    s = " ".join(f"{u},{v}" for u, v in cs)
    print(s if want is not None else f"{n} {s}")
