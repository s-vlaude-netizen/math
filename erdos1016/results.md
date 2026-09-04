# Results — Erdős 1016 / OEIS A105206

$h(n)$ = smallest $h$ such that some graph on $n$ vertices with $n+h$ edges is
pancyclic. A105206 lists $a(n)=n+h(n)$.

All values below are **exhaustive**: for each $n$, every chord set of size
$k=h(n)-1$ was refuted (up to rotation) and a witness with $k=h(n)$ was found.
Every witness was re-verified independently by enumerating all simple cycles
with networkx (`verify_pancyclic.py witness`).

## Published terms, reproduced

$n=3\ldots22$: `3, 5, 6, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26`
— all 20 reproduced by the searcher.

*(The OEIS name says "on n+2 vertices"; the data and the entry's own example are
indexed by the number of vertices itself, which is the reading used here.)*

## New terms

| $n$ | $h(n)$ | $a(n)=n+h(n)$ | witness (chords added to $C_n$) |
|---:|---:|---:|---|
| 23 | 4 | **27** | (0,2), (0,20), (1,18), (8,20) |
| 24 | 4 | **28** | (0,2), (0,21), (1,19), (8,23) |
| 25 | 5 | **30** | (0,2), (0,3), (0,4), (0,20), (1,16) |
| 26 | 5 | **31** | (0,2), (0,3), (0,4), (1,11), (1,22) |
| 27 | 5 | **32** | (0,2), (0,3), (0,5), (1,9), (3,18) |
| 28 | 5 | **33** | (0,2), (0,3), (0,5), (1,17), (2,21) |
| 29 | 5 | **34** | (0,2), (0,3), (0,5), (1,18), (2,22) |
| 30 | 5 | **35** | (0,2), (0,3), (0,9), (3,11), (4,16) |
| 31 | 5 | **36** | (0,2), (0,3), (0,10), (5,9), (6,24) |
| 32 | 5 | **37** | (0,2), (0,3), (0,28), (1,14), (6,31) |
| 33 | 5 | **38** | (0,2), (0,3), (0,29), (1,14), (6,32) |
| 34 | 5 | **39** | (0,2), (0,4), (0,21), (1,7), (3,26) |

The two hardest refutations were $n=25$, $k=4$ (233 829 950 nodes without
symmetry reduction, 10 997 097 with it) and the $k=5$ searches beyond $n=34$.

For $n\ge 34$ the lower bound $h(n)\ge 5$ is free: $2^{5}-1=31 < n-2$, so four
chords cannot supply enough cycles. Below that it had to be proved by search.

## What the data says about the open question

Erdős asked whether $h(n)\ge\log_2 n+\log_* n-O(1)$, and remarked that he could
not even prove $h(n)-\log_2 n\to\infty$. Let $N(k)$ be the largest $n$ with
$h(n)=k$ — the point where $h$ is furthest below $\log_2 n$.

| $k$ | 0 | 1 | 2 | 3 | 4 | 5 |
|---|---:|---:|---:|---:|---:|---:|
| $N(k)$ | 3 | 5 | 8 | 14 | 24 | $\ge 34$ |
| Bondy's counting bound $2^{k+1}+1$ | 3 | 5 | 9 | 17 | 33 | 65 |
| $N(k)/2^k$ | 3 | 2.5 | 2 | 1.75 | 1.5 | $\ge 1.06$ |
| $k-\log_2 N(k)$ | −1.58 | −1.32 | −1.00 | −0.81 | −0.58 | $\le -0.09$ |

$N(k)/2^k$ decreases at every step of the computed range. If that persists,
$h(n)-\log_2 n\to\infty$, which is what Erdős believed but could not prove; the
counting bound alone would allow $N(k)/2^k\approx 2$ forever. Six data points
prove nothing, but every one of them points the same way, and the gap to Bondy's
bound widens monotonically ($0,0,1,3,9,\ge 31$).
