#!/usr/bin/env python3
"""
symcheck7.py TYPESFILE SOLFILES... : closure of the solution list under the symmetries of each type.
A solution is (type, arc vector).  Every dihedral map (rotation/reflection of the p positions) that maps the type's chord set to
itself maps a solution to another solution of the same type.  If a pruning rule treated such images differently, some image would be
missing.  Reads "S idx n a_0..a_{p-1} C ..." lines.
"""
import glob, sys
types = {}
for line in open(sys.argv[1]):
    t = line.split(); idx, p = int(t[0]), int(t[1]); e = t.index("E"); ed = list(map(int, t[e + 1:]))
    types[idx] = (p, frozenset(tuple(sorted((ed[i], ed[i + 1]))) for i in range(0, len(ed), 2)))
sols = {}
for pat in sys.argv[2:]:
    for f in glob.glob(pat):
        for line in open(f):
            if line.startswith("S "):
                t = line.split(); w = t[1:t.index("C")] if "C" in t else t[1:]
                idx = int(w[0]); n = int(w[1]); arcs = tuple(map(int, w[2:]))
                sols.setdefault(idx, set()).add(arcs)
miss = total = autos = 0
for idx, S in sols.items():
    p, E = types[idx]
    maps = []
    for fl in (0, 1):
        for r in range(p):
            m = [((-i if fl else i) + r) % p for i in range(p)]
            if frozenset(tuple(sorted((m[a], m[b]))) for a, b in E) == E: maps.append((fl, r))
    autos += len(maps) > 1
    for arcs in S:
        total += 1
        for fl, r in maps:
            # position i goes to m(i); arc i (between positions i and i+1) goes to the arc between m(i) and m(i+1)
            new = [0] * p
            for i in range(p):
                if fl == 0: new[(i + r) % p] = arcs[i]
                else: new[(-(i + 1) + r) % p] = arcs[i]
            if tuple(new) not in S:
                miss += 1
                if miss <= 5: print("MISSING image", idx, arcs, "->", tuple(new))
print(f"{len(sols)} types, {total} solutions, {autos} types with a nontrivial symmetry; missing images: {miss}")
