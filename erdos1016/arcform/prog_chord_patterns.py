from itertools import combinations
def a(k):  # chord patterns: k distinct chords on p points of a circle covering all points, up to rotation/reflection, all p
    t = 0
    for p in range(2, 2*k + 1):
        seen = set()
        for c in combinations(list(combinations(range(p), 2)), k):
            if len({v for e in c for v in e}) < p: continue
            seen.add(min(tuple(sorted(tuple(sorted(((s*v + r) % p) for v in e)) for e in c)) for r in range(p) for s in (1, -1)))
        t += len(seen)
    return t
print([a(k) for k in range(1, 6)])
