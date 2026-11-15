from itertools import product
def thr(n):  # truth tables of all threshold functions of n variables (weights |w| <= 3 suffice for n <= 4)
    pts = list(product((0, 1), repeat=n))
    out = set()
    for w in product(range(-3, 4), repeat=n):
        d = [sum(a * b for a, b in zip(w, p)) for p in pts]
        for t in set(d) | {max(d) + 1}:
            out.add(sum(1 << i for i, v in enumerate(d) if v >= t))
    return out
def a(n):  # functions that are the AND of two threshold functions
    H = thr(n); full = (1 << 2**n) - 1
    S = {x & y for x in H for y in H}
    return len(S)
print([a(n) for n in range(1, 5)])
