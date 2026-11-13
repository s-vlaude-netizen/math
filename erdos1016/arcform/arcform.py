#!/usr/bin/env python3
"""
arcform.py -- pancyclic cycle-plus-chords graphs, described without reference to n.

A chord set on C_n is determined by
  * its TYPE: the p distinct chord endpoints in cyclic order (positions 0..p-1) and which
    pairs of positions are joined by a chord (k chords, every position used), and
  * its ARC LENGTHS a_0..a_{p-1} >= 1: a_i is the distance from position i to position i+1
    along the cycle, so n = a_0 + ... + a_{p-1}.

Every cycle of the graph is a cycle of the "skeleton" (positions, arcs, chords), and its
length is  (number of chords used) + (sum of the arc lengths used).  So for a fixed type
there are at most 2^(k+1)-1 linear forms  m_j + sum_{i in A_j} a_i,  and the graph is
pancyclic iff these forms take every value 3..n.  The number of types depends only on k.

    python3 arcform.py count K            number of types and of cycle forms
    python3 arcform.py dump K             the types with their forms, one per line, for arcdfs.c
    python3 arcform.py brute K N          every pancyclic chord set on C_N with K chords,
                                          one per line (all types, all arc vectors)
"""
import sys
from itertools import combinations


def gen_labeled(p, k):
    """All sets of k distinct pairs on positions 0..p-1 that use every position."""
    pairs = list(combinations(range(p), 2))
    out = []

    def rec(start, chosen, covered):
        r = k - len(chosen)
        if r == 0:
            if len(covered) == p:
                out.append(tuple(chosen))
            return
        if p - len(covered) > 2 * r:
            return
        for idx in range(start, len(pairs)):
            i, j = pairs[idx]
            # later pairs have first element >= i, so every position below i must be covered already
            if any(v not in covered for v in range(i)):
                break
            rec(idx + 1, chosen + [(i, j)], covered | {i, j})

    rec(0, [], frozenset())
    return out


def canon(p, edges):
    best = None
    for fl in (0, 1):
        for t in range(p):
            img = tuple(sorted(tuple(sorted((((p - u) % p if fl else u) + t) % p for u in e)) for e in edges))
            if best is None or img < best:
                best = img
    return best


def gen_types(k):
    """Canonical types (p, edges) for k chords, all p."""
    types = []
    for p in range(2, 2 * k + 1):
        if p * (p - 1) // 2 < k:
            continue
        seen = set()
        for edges in gen_labeled(p, k):
            c = canon(p, edges)
            if c not in seen:
                seen.add(c)
                types.append((p, c))
    return types


def lower_bounds(p, edges):
    """a_i >= 1, and >= 2 where a chord joins the two ends of arc i (it must not be a cycle edge)."""
    lo = [1] * p
    for (i, j) in edges:
        if j == i + 1:
            lo[i] = 2
        if i == 0 and j == p - 1:
            lo[p - 1] = 2
    return lo


def cycle_forms(p, edges):
    """All cycles of the skeleton other than the Hamiltonian one: list of (chords used, frozenset of arcs)."""
    k = len(edges)
    ends = [(i, (i + 1) % p) for i in range(p)] + list(edges)        # arcs 0..p-1, then chords
    fund = [frozenset(range(p))] + [frozenset(range(i, j)) | {p + c} for c, (i, j) in enumerate(edges)]
    forms = []
    for mask in range(2, 1 << (k + 1)):
        s = set()
        for b in range(k + 1):
            if mask >> b & 1:
                s ^= fund[b]
        inc = {}
        for e in s:
            for v in ends[e]:
                inc.setdefault(v, []).append(e)
        if not s or any(len(x) != 2 for x in inc.values()):
            continue
        start = next(iter(inc))
        prev_e, cur, steps = None, start, 0
        while True:
            e = inc[cur][0] if inc[cur][0] != prev_e else inc[cur][1]
            a, b2 = ends[e]
            cur = b2 if a == cur else a
            prev_e = e
            steps += 1
            if cur == start:
                break
        if steps == len(s):
            forms.append((sum(1 for e in s if e >= p), frozenset(e for e in s if e < p)))
    return forms


def compositions(n, lo):
    """All integer vectors a with a_i >= lo_i and sum n."""
    p = len(lo)

    def rec(i, left):
        if i == p - 1:
            if left >= lo[i]:
                yield (left,)
            return
        need = sum(lo[i + 1:])
        for v in range(lo[i], left - need + 1):
            for rest in rec(i + 1, left - v):
                yield (v,) + rest

    yield from rec(0, n)


def chord_set(edges, a):
    pos = [0]
    for x in a[:-1]:
        pos.append(pos[-1] + x)
    return sorted(tuple(sorted((pos[i], pos[j]))) for i, j in edges)


if __name__ == "__main__":
    cmd, k = sys.argv[1], int(sys.argv[2])
    types = gen_types(k)
    if cmd == "count":
        nf = [len(cycle_forms(p, e)) for p, e in types]
        byp = {}
        for (p, e) in types:
            byp[p] = byp.get(p, 0) + 1
        print(f"k={k}: {len(types)} types; by number of endpoints {dict(sorted(byp.items()))}; "
              f"cycles besides the Hamiltonian one: max {max(nf)} of {2 ** (k + 1) - 2}")
    elif cmd == "dump":
        # one line per type for arcdfs.c:  idx p F  lo_0..lo_{p-1}  (m mask)*F  E  (i j)*k
        for idx, (p, edges) in enumerate(types):
            lo = lower_bounds(p, edges)
            forms = sorted({(m, sum(1 << i for i in A)) for m, A in cycle_forms(p, edges)})
            print(idx, p, len(forms), *lo, *[x for f in forms for x in f], "E", *[x for e in edges for x in e])
    elif cmd == "brute":
        n = int(sys.argv[3])
        for p, edges in types:
            if p > n:
                continue
            lo = lower_bounds(p, edges)
            forms = cycle_forms(p, edges)
            if len(forms) < n - 3:
                continue
            for a in compositions(n, lo):
                vals = {m + sum(a[i] for i in A) for m, A in forms}
                if all(L in vals for L in range(3, n)):
                    print(" ".join(f"{u},{v}" for u, v in chord_set(edges, a)))
