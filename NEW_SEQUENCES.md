# Drafts for new OEIS sequences (checked 2026-11-15: no match in the OEIS)

Submit via https://oeis.org/Submit.html, option 1 ("Contribute a new sequence").
Both programs below were run and reproduce the terms they print.

---

## 1. Largest n with a sparse pancyclic graph

**NAME**
Largest n such that there is a pancyclic graph with n vertices and n+k edges.

**DATA** (offset 0)
```
3, 5, 8, 14, 24, 40, 67
```
a(6) = 67 is confirmed by two independently written solvers (`erdos1016/arcform/xcheck/`). a(7) = 114 comes from
one program only; add it only once a second program has confirmed it.

**COMMENTS**
```
Finite by Bondy's counting bound: a pancyclic graph on n vertices with n+k edges is a Hamiltonian cycle plus k chords and has at most 2^(k+1)-1 cycles, so a(k) <= 2^(k+1)+1.
Equivalently, a(k) is the largest m with A105206(m) - m <= k (for the known terms h = A105206(m) - m is nondecreasing).
Terms were computed by exhaustive computer search over all cyclic patterns of the chord endpoints, which decides all n at once.
```

**PROG**
```
(Python)
from itertools import combinations
def pancyclic(n, c):
    adj = [{(v-1) % n, (v+1) % n} for v in range(n)]
    for i, j in c: adj[i].add(j); adj[j].add(i)
    L = set()
    def dfs(s, v, seen):
        for w in adj[v]:
            if w == s and len(seen) > 2: L.add(len(seen))
            elif w > s and w not in seen: dfs(s, w, seen | {w})
    for s in range(n): dfs(s, s, {s})
    return L >= set(range(3, n+1))
def a(k):
    for n in range(2**(k+1) + 1, 2, -1):
        ch = [(i, j) for i in range(n) for j in range(i+2, n) if (i, j) != (0, n-1)]
        if any(pancyclic(n, c) for c in combinations(ch, k)): return n
print([a(k) for k in range(0, 3)])  # ~~~~
```

**CROSSREFS** A105206.   **KEYWORD** nonn,hard,more

---

## 2. Chord patterns on a circle

**NAME**
Number of ways to place k distinct chords between points on a circle so that every point is an endpoint, up to rotation and reflection (any number of points).

**DATA** (offset 1)
```
1, 3, 14, 103, 1236, 21878, 496875, 13550514
```

**COMMENTS**
```
Equivalently: graphs with k edges and no isolated vertices whose vertices are placed in cyclic order, counted up to the dihedral group. The labeled analog is A121251.
a(1)..a(6) agree between two independent programs; a(7) and a(8) were checked by verifying that the orbit sizes sum to A121251(k).
```

**EXAMPLE**
```
a(2) = 3: two chords on 4 points either cross or not, and two chords sharing one of 3 points.
```

**PROG**
```
(Python)
from itertools import combinations
def a(k):
    t = 0
    for p in range(2, 2*k + 1):
        seen = set()
        for c in combinations(list(combinations(range(p), 2)), k):
            if len({v for e in c for v in e}) < p: continue
            seen.add(min(tuple(sorted(tuple(sorted(((s*v + r) % p) for v in e)) for e in c)) for r in range(p) for s in (1, -1)))
        t += len(seen)
    return t
print([a(k) for k in range(1, 6)])  # ~~~~
```

**CROSSREFS** A121251.   **KEYWORD** nonn,more

---

## More drafts (added 2026-11-15)

* `nn-sequences/DRAFTS.md`: Boolean functions computable by one-hidden-layer threshold networks with two hidden neurons
  (4, 16, 254, 41614, 74986552), ANDs of two threshold functions (4, 16, 246, 32506, 44642388), and the irregular triangle
  "how many hidden neurons are needed" for n <= 4.
* `threshold-graphs/DRAFTS.md`: graphs with threshold dimension at most 2, labeled (1, 2, 8, 64, 1012, 28084, 1169611,
  65535747) and unlabeled (1, 2, 4, 11, 33, 133, 660, 4090).
* `oeis-programs/PROGRAMS.md`: Python programs for 14 existing entries that had none.
