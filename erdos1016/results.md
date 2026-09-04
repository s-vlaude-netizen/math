# Results — Erdős 1016 / OEIS A105206

$h(n)$ = smallest $h$ such that some graph on $n$ vertices with $n+h$ edges is
pancyclic. A105206 lists $a(n)=n+h(n)$.

Each value needs two halves: a **witness** with $h$ chords, and a **lower bound**
ruling out $h-1$.

* For $n\le 33$ the lower bound had to be proved by exhaustive search over every
  chord set of size $h-1$, up to rotation.
* For $n\ge 34$ it is free: $2^{5}-1=31<n-2$, so four chords cannot even supply
  enough cycles (Bondy's counting bound), and only a witness is needed.

Every witness was re-verified independently by enumerating **all** simple cycles
with networkx, i.e. without using the cycle-space argument
(`python3 verify_all.py` re-derives the whole table).

## Published terms, reproduced

$n=3\ldots22$: `3, 5, 6, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26`
— all 20 reproduced by the searcher before any new value was trusted.

*(The OEIS name says "on n+2 vertices"; the data and the entry's own example are
indexed by the number of vertices itself, which is the reading used here.)*

## New terms

| $n$ | $h(n)$ | $a(n)$ | witness (chords added to $C_n$) | lower bound |
|---:|---:|---:|---|---|
| 23 | 4 | **27** | (0,2) (0,20) (1,18) (8,20) | exhaustive |
| 24 | 4 | **28** | (0,2) (0,21) (1,19) (8,23) | exhaustive |
| 25 | 5 | **30** | (0,2) (0,3) (0,4) (0,20) (1,16) | exhaustive |
| 26 | 5 | **31** | (0,2) (0,3) (0,4) (1,11) (1,22) | exhaustive |
| 27 | 5 | **32** | (0,2) (0,3) (0,5) (1,9) (3,18) | exhaustive |
| 28 | 5 | **33** | (0,2) (0,3) (0,5) (1,17) (2,21) | exhaustive |
| 29 | 5 | **34** | (0,2) (0,3) (0,5) (1,18) (2,22) | exhaustive |
| 30 | 5 | **35** | (0,2) (0,3) (0,9) (3,11) (4,16) | exhaustive |
| 31 | 5 | **36** | (0,2) (0,3) (0,10) (5,9) (6,24) | exhaustive |
| 32 | 5 | **37** | (0,2) (0,3) (0,28) (1,14) (6,31) | exhaustive |
| 33 | 5 | **38** | (0,2) (0,3) (0,29) (1,14) (6,32) | exhaustive |
| 34 | 5 | **39** | (0,2) (0,4) (0,21) (1,7) (3,26) | counting |
| 35 | 5 | **40** | (0,2) (0,10) (1,4) (3,18) (6,34) | counting |
| 36 | 5 | **41** | (0,2) (0,11) (1,7) (7,10) (8,19) | counting |
| 37 | 5 | **42** | (9,26) (14,24) (25,28) (9,22) (23,25) | counting |
| 38 | 5 | **43** | (1,24) (0,35) (24,26) (25,37) (19,36) | counting |
| 39 | 5 | **44** | (13,31) (11,14) (12,28) (19,30) (29,31) | counting |
| 40 | 5 | **45** | (20,29) (28,31) (10,25) (26,28) (27,33) | counting |

The hardest refutation was $n=25$, $k=4$: 233 829 950 nodes without the symmetry
reduction, 10 997 097 with it — both runs agreeing, which is how the reduction
was checked. The same cross-check at $n=26$ gave 311 112 744 vs 13 906 189 nodes,
again the same verdict.

## Two search modes

`pancyclic.c` is the exhaustive searcher and is what proves lower bounds.
`heur.c` only looks for witnesses, by hill-climbing on "how many of the lengths
$3..n$ does this chord set realise". Since scoring a chord set is a few thousand
operations, it explores far more of the space: at $n=36$ the exhaustive search
needed 276 249 985 nodes and 12 minutes to reach a witness that hill climbing
finds in 57 000 evaluations. Every witness it produced was then checked
independently.

## What the data says about the open question

Erdős asked whether $h(n)\ge\log_2 n+\log_* n-O(1)$, and remarked he could not
even prove $h(n)-\log_2 n\to\infty$. Let $N(k)$ be the largest $n$ with $h(n)=k$
— the point where $h$ falls furthest below $\log_2 n$.

| $k$ | 0 | 1 | 2 | 3 | 4 | 5 |
|---|---:|---:|---:|---:|---:|---:|
| $N(k)$ | 3 | 5 | 8 | 14 | 24 | $\ge 40$ |
| Bondy's counting bound $2^{k+1}+1$ | 3 | 5 | 9 | 17 | 33 | 65 |
| $N(k)/2^k$ | 3 | 2.5 | 2 | 1.75 | 1.5 | $\ge 1.25$ |
| $k-\log_2 N(k)$ | −1.58 | −1.32 | −1.00 | −0.81 | −0.58 | $\le -0.32$ |

$N(k)/2^k$ falls at every step of the computed range, and the gap to Bondy's
bound widens monotonically ($0,0,1,3,9,\le 25$). If that persists,
$h(n)-\log_2 n\to\infty$ — the direction Erdős believed but could not prove;
the counting bound by itself would happily allow $N(k)/2^k$ to stay near 2
forever. Six data points prove nothing, but all six point the same way.
