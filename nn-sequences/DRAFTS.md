# Drafts: sequences about threshold networks (checked 2026-11-15: none of these is in the OEIS)

Submit via https://oeis.org/Submit.html, option 1. A "threshold function" of n variables is a Boolean function
[w.x >= t] with real weights w and threshold t (A000609 counts them: 2, 4, 14, 104, 1882, 94572, ...).
A "one-hidden-layer threshold network with k hidden neurons" computes g(h_1,...,h_k) where every h_i is a threshold
function of the n inputs and g is a threshold function of the k hidden outputs.

How the numbers were obtained and checked: `README.md` in this folder. The n = 5 terms are marked: they come from
one C program (a table of 2^32 bits) and an independent random-sample check; see the status line of each draft.

---

## 1. Two hidden neurons

**NAME** Number of Boolean functions of n variables that can be computed by a neural network with one hidden layer of two threshold neurons.

**DATA** (offset 1)  `4, 16, 254, 41614, 74986552`

**COMMENTS**
```
All neurons are linear threshold units; the output neuron reads only the two hidden neurons. The set of threshold functions is closed under negation, so the output neuron can be taken to be an AND or an OR: a(n) counts the Boolean functions f such that f or its complement is the AND of two threshold functions.
a(n) >= A000609(n), with equality only for n <= 1. The 2^(2^n) - a(n) functions not counted are those needing at least 3 hidden neurons: 0, 0, 2, 23922, 4219980744. For n = 3 these are the two parity functions; for n = 4 the two parity functions are the only ones needing 4 hidden neurons (see the triangle).
```

**PROG**
```
(Python)
from itertools import product
def thr(n):  # truth tables of all threshold functions of n variables (weights |w| <= 3 suffice for n <= 4)
    pts = list(product((0, 1), repeat=n))
    out = set()
    for w in product(range(-3, 4), repeat=n):
        d = [sum(a * b for a, b in zip(w, p)) for p in pts]
        for t in set(d) | {max(d) + 1}:
            out.add(sum(1 << i for i, v in enumerate(d) if v >= t))
    return out
def a(n):  # functions computable with two hidden neurons: AND of two threshold functions, or its complement
    H = thr(n); full = (1 << 2**n) - 1
    S = {x & y for x in H for y in H}
    return len(S | {full ^ x for x in S})
print([a(n) for n in range(1, 5)])  # ~~~~
```

**CROSSREFS** Cf. A000609, A064436.   **KEYWORD** nonn,hard,more,   **STATUS of n=5 term:** see README.

---

## 2. AND of two threshold functions

**NAME** Number of Boolean functions of n variables that are the AND of two threshold functions.

**DATA** (offset 1)  `4, 16, 246, 32506, 44642388`

**COMMENTS**
```
Equivalently, subsets of the vertices of the n-cube that are cut out by two closed half-spaces: the vertex set intersected with two half-spaces.
a(3) = 246: the 10 functions not counted are the two parity functions (4 true points) and 8 functions with exactly 5 true points. a(4) = 32506 = 65536 - 33030.
```

**PROG**
```
(Python)
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
print([a(n) for n in range(1, 5)])  # ~~~~
```

**CROSSREFS** Cf. A000609, sequence 1 above.   **KEYWORD** nonn,hard,more

---

## 3. Irregular triangle: how many hidden neurons

**NAME** Irregular triangle read by rows: T(n,k) is the number of Boolean functions of n variables that can be computed by a one-hidden-layer threshold network with k hidden neurons but not with k-1.

**DATA** (offset 1, rows n = 1..4)  `4, 14, 2, 104, 150, 2, 1882, 39732, 23920, 2`

**COMMENTS**
```
Row sums are 2^(2^n). Column 1 is A000609 (threshold functions). T(n,2) is sequence 1 minus A000609.
The n-bit parity functions need n hidden neurons for n <= 4: T(n,n) = 2 for n = 2, 3, 4. For n = 5 the row has at least 5 entries; its first two entries are 94572 and 74891980.
Rows 1..3 can be computed by exhaustive search (program below); row 4 was computed by classifying the 65536 functions into the 222 classes under permutation and negation of inputs and output, and deciding each class with an exact integer-programming solver (every network found was re-evaluated; infeasibility rests on CP-SAT).
```

**PROG**
```
(Python)
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
print([row(n) for n in range(1, 4)])  # ~~~~
```

**CROSSREFS** Cf. A000609, A064436.   **KEYWORD** nonn,tabf,more
