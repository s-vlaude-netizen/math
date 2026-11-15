from itertools import permutations, product
def a(n):  # labeled graphs that are the union of two threshold graphs on the same n vertices
    T = set()  # threshold graphs: add vertices one at a time as isolated (0) or dominating (1)
    for p in permutations(range(n)):
        for t in product((0, 1), repeat=n - 1):
            T.add(frozenset((min(p[j], p[k]), max(p[j], p[k])) for k in range(1, n) if t[k - 1] for j in range(k)))
    return len({g | h for g in T for h in T})
print([a(n) for n in range(1, 6)])
