# Results — Erdős 357 / OEIS A364132, A364153

Recall the reformulation (derivation in `README.md`):

* **A364153(k)** = min over Golomb rulers with $k+1$ marks of the largest gap
  (this is the inverse of $g$, the non-monotone case);
* **A364132(k)** = the same with the gaps required to increase
  (inverse of $f$, Erdős's original monotone question).

Every value below is **exhaustive**: for $m<a(k)$ the searcher proved that no
such ruler exists, and for $m=a(k)$ it produced the witness shown. Every witness
was re-checked independently by expanding all $k(k+1)/2$ consecutive sums and
testing distinctness (`verify.py`).

## Published terms, reproduced

* A364153, $k=1..13$: `1, 2, 3, 5, 6, 7, 9, 10, 12, 13, 14, 17, 18` ✓
* A364132, $k=1..22$: `1, 2, 4, 5, 7, 10, 12, 13, 15, 18, 21, 24, 25, 29, 30, 33, 36, 38, 41, 47, 50, 52` ✓

## New terms

### A364153 (arbitrary order)

| $k$ | $a(k)$ | witness $(a_1,\dots,a_k)$ | refuted below |
|---:|---:|---|---|
| 14 | **20** | 1, 4, 6, 17, 15, 14, 20, 16, 19, 18, 8, 13, 9, 3 | $m=18$ (54 492 130 nodes), $m=19$ (149 445 063) |
| 15 | **22** | 1, 11, 16, 8, 21, 22, 20, 19, 15, 18, 14, 17, 6, 3, 4 | $m=20$ (407 627 538), $m=21$ (1 099 325 340) |
| 16 | **24** | 1, 2, 6, 5, 23, 20, 24, 22, 18, 12, 21, 17, 15, 10, 16, 19 | $m=22$ (2 973 532 126), $m=23$ (7 920 625 479) |

### A364132 (increasing)

| $k$ | $a(k)$ | witness $(a_1<\dots<a_k)$ | refuted below |
|---:|---:|---|---|
| 23 | **56** | 1, 4, 6, 8, 24, 27, 33, 35, 36, 37, 39, 40, 41, 44, 45, 46, 48, 49, 52, 53, 54, 55, 56 | $m=52,53,54,55$ (up to 566 123 991 nodes) |
| 24 | **60** | 1, 6, 12, 15, 30, 31, 36, 37, 40, 41, 42, 43, 44, 46, 47, 49, 50, 53, 54, 55, 56, 58, 59, 60 | $m=56,\dots,59$ (up to 2 193 364 619 nodes) |
| 25 | **63** | 2, 4, 8, 10, 13, 33, 34, 41, 42, 43, 44, 45, 47, 48, 49, 51, 52, 53, 54, 55, 57, 59, 61, 62, 63 | $m=60,61,62$ (up to 5 199 898 929 nodes) |
| 26 | **67** | 1, 2, 4, 5, 14, 22, 35, 39, 42, 44, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 61, 63, 64, 65, 66, 67 | $m=63,\dots,66$ (up to 18 859 978 541 nodes) |

## The functions Erdős actually asked about

Inverting the sequences gives $f$ and $g$ themselves.

$f(n)$, $n=1\ldots67$ (monotone):

```
1 2 2 3 4 4 5 5 5 6 6 7 8 8 9 9 9 10 10 10 11 11 11 12 13 13 13 13 14 15 15 15 16 16 16 17 17 18 18 18 19 19 19 19 19 19 20 20 20 21 21 22 22 22 22 23 23 23 23 24 24 24 25 25 25 25 26
```

$g(n)$, $n=1\ldots24$ (arbitrary order):

```
1 2 3 3 4 5 6 6 7 8 8 9 10 11 11 11 12 13 13 14 14 15 15 16
```

Erdős asked whether $f(n)=o(n)$. At the record points the ratio $k/a(k)$ is

| $k$ | 10 | 12 | 15 | 18 | 20 | 22 | 23 | 24 | 25 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| $f$-ratio | .556 | .500 | .500 | .474 | .426 | .423 | **.411** | **.400** | **.397** (k=25), **.388** (k=26) |
| $g$-ratio | .769 | .706 | .682 (k=15), .667 (k=16) | — | — | — | — | — | — |

The monotone ratio decreases essentially monotonically over the whole computed
range and the two new terms continue it; the best proven lower bound in the
monotone case is only $f(n)\ge(2+o(1))\sqrt n$ (via Erdős problem 874), which at
$n=60$ would give just 15 against the true value 24. For $g$ the known bounds
are $(\frac13+o(1))n\le g(n)\le(\frac23-\frac1{512}+o(1))n$; the computed
$g$-ratios sit just above $\frac23$ and are still falling, so the data is
consistent with the upper bound being approached from above rather than with
$g(n)/n$ settling near $\frac13$.
