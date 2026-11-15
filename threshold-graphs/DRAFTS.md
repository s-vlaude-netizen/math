# Drafts: graphs of threshold dimension at most 2 (checked 2026-11-15: not in the OEIS)

The *threshold dimension* of a graph G (Chvatal and Hammer) is the least number of threshold graphs on the node set of G
whose union is G (some papers use intersections instead; the two are related by complementation, so the counts below are
the same for either convention). Dimension at most 2 is decidable in polynomial time. A threshold graph is built from one
node by repeatedly adding an isolated or a dominating node (A005840 counts the labeled ones: 1, 2, 8, 46, 332, 2874, 29024, 334982).

How the terms were obtained and checked: `README.md` in this folder.

---

## 1. Labeled

**NAME** Number of labeled graphs on n nodes with threshold dimension at most 2, i.e. that are the union of two threshold graphs on the same node set.

**DATA** (offset 1)  `1, 2, 8, 64, 1012, 28084, 1169611, 65535747`

**COMMENTS**
```
The complement of a threshold graph is a threshold graph, so a(n) is also the number of labeled graphs that are the intersection of two threshold graphs.
Every graph on at most 4 nodes qualifies. The only graphs on 5 nodes that do not are the 12 labeled 5-cycles, so a(5) = 2^10 - 12.
a(n) >= A005840(n), the number of labeled threshold graphs, and a(n) <= 2^(n(n-1)/2).
```

**PROG**
```
(Python)
from itertools import permutations, product
def a(n):  # labeled graphs that are the union of two threshold graphs on the same n vertices
    T = set()  # threshold graphs: add vertices one at a time as isolated (0) or dominating (1)
    for p in permutations(range(n)):
        for t in product((0, 1), repeat=n - 1):
            T.add(frozenset((min(p[j], p[k]), max(p[j], p[k])) for k in range(1, n) if t[k - 1] for j in range(k)))
    return len({g | h for g in T for h in T})
print([a(n) for n in range(1, 6)])  # ~~~~
```

**EXTENSIONS** `a(6)-a(8) from ~~~~`   (C program: all pairs of labeled threshold graphs, marked in a table of 2^(n(n-1)/2) bits)

**CROSSREFS** Cf. A005840, A000088.   **KEYWORD** nonn,more

---

## 2. Unlabeled

**NAME** Number of graphs on n nodes with threshold dimension at most 2.

**DATA** (offset 1)  `1, 2, 4, 11, 33, 133, 660, 4090`

**COMMENTS**
```
a(n) = A000088(n) for n <= 4. a(5) = A000088(5) - 1: the 5-cycle is the only graph on 5 nodes with threshold dimension 3. a(6) = 156 - 23 = 133.
Counting isomorphism classes of the graphs in A(labeled version); computed with Burnside's lemma over the cycle types of the symmetric group.
```
(Before submitting, replace the second comment line by: "Isomorphism classes of the graphs counted by the labeled version (link to it)".)

**PROG**
```
(Python)
from itertools import permutations, product
def a(n):  # isomorphism classes of graphs that are the union of two threshold graphs on n vertices
    T = set()
    for p in permutations(range(n)):
        for t in product((0, 1), repeat=n - 1):
            T.add(frozenset((min(p[j], p[k]), max(p[j], p[k])) for k in range(1, n) if t[k - 1] for j in range(k)))
    U = {g | h for g in T for h in T}
    canon = lambda g: min(tuple(sorted(tuple(sorted((s[u], s[v]))) for u, v in g)) for s in permutations(range(n)))
    return len({canon(g) for g in U})
print([a(n) for n in range(1, 6)])  # ~~~~
```

**EXTENSIONS** `a(6)-a(8) from ~~~~`

**CROSSREFS** Cf. A000088, A005840, A123473 (agrees for n <= 6, differs at n = 7: 598 there, 660 here).   **KEYWORD** nonn,more
