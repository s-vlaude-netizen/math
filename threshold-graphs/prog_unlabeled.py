from itertools import permutations, product
def a(n):  # isomorphism classes of graphs that are the union of two threshold graphs on n vertices
    T = set()
    for p in permutations(range(n)):
        for t in product((0, 1), repeat=n - 1):
            T.add(frozenset((min(p[j], p[k]), max(p[j], p[k])) for k in range(1, n) if t[k - 1] for j in range(k)))
    U = {g | h for g in T for h in T}
    canon = lambda g: min(tuple(sorted(tuple(sorted((s[u], s[v]))) for u, v in g)) for s in permutations(range(n)))
    return len({canon(g) for g in U})
print([a(n) for n in range(1, 6)])
