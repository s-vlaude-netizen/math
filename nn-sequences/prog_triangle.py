from itertools import product
def thr(n):  # truth tables of all threshold functions of n variables (weights |w| <= 3 suffice for n <= 3)
    pts = list(product((0, 1), repeat=n))
    out = set()
    for w in product(range(-3, 4), repeat=n):
        d = [sum(a * b for a, b in zip(w, p)) for p in pts]
        for t in set(d) | {max(d) + 1}:
            out.add(sum(1 << i for i, v in enumerate(d) if v >= t))
    return out
def row(n):  # rows n <= 3; hidden-layer sizes 1, 2, 3
    H = thr(n); full = (1 << 2**n) - 1
    S2 = {x & y for x in H for y in H}; S2 |= {full ^ x for x in S2}
    S3 = set(S2)
    for a, b, c in product(H, repeat=3):  # a threshold function of 3 variables is, up to negations, x, x&y, x&y&z, maj, x&(y|z)
        for r in (a & b & c, (a & b) | (a & c) | (b & c), a & (b | c)):
            S3.add(r); S3.add(full ^ r)
    t = [len(H), len(S2) - len(H), len(S3) - len(S2)]
    while t[-1] == 0: t.pop()
    return t
print([row(n) for n in range(1, 4)])
