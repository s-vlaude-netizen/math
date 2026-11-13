# Results — Erdős 1016 / OEIS A105206

$h(n)$ = smallest $h$ such that some graph on $n$ vertices with $n+h$ edges is
pancyclic. A105206 lists $a(n)=n+h(n)$.

Each value needs two halves: a **witness** with $h$ chords, and a **lower bound**
ruling out $h-1$.

Three kinds of lower bound are used:

* **$n\le 33$:** exhaustive search over every chord set of size $h-1$ on $C_n$, up
  to rotation (`pancyclic.c`, `pancyclic2.c`).
* **$34\le n\le 40$:** free, because $2^{5}-1=31<n-2$: four chords cannot even
  supply enough cycles (Bondy's counting bound).
* **$n\ge 41$:** a search that does not fix $n$ at all (`arcform/`). It shows that
  five chords never suffice for $n\ge 41$ and six chords never for $n\ge 68$.

Every witness was re-verified independently by enumerating **all** simple cycles
with networkx, i.e. without using the cycle-space argument
(`python3 verify_all.py` re-derives the whole table).

## All values now known

| $n$ | 3 | 4–5 | 6–8 | 9–14 | 15–24 | 25–40 | 41–67 | 68–114 | $\ge 115$ |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| $h(n)$ | 0 | 1 | 2 | 3 | 4 | 5 | **6** | **7** | $\ge 8$ |

So $a(n)=n+h(n)$ runs through every integer from 3 to 121 except
4, 7, 11, 18, 29, 46, 74. The OEIS entry stops at $n=22$; $n=23..40$ came from the
per-$n$ search below, $n=41..114$ from `arcform/`.

## Published terms, reproduced

$n=3\ldots22$: `3, 5, 6, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26`
— all 20 reproduced by the searcher before any new value was trusted.

*(The OEIS name says "on n+2 vertices"; the data and the entry's own example are
indexed by the number of vertices itself, which is the reading used here.)*

## New terms, $n=23..40$

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

## New terms, $n=41..114$

$a(n)=n+6$ for $41\le n\le 67$ (47, 48, ..., 73) and $a(n)=n+7$ for
$68\le n\le 114$ (75, 76, ..., 121).

*Witnesses.* One for every $n$, in `arcform/witnesses_k6_n41_67.json` and
`arcform/witnesses_k7_n68_114.json`, each checked with networkx by enumerating
all simple cycles. The largest ones:

| $n$ | chords added to $C_n$ |
|---:|---|
| 67 | (0,2) (0,60) (1,13) (3,61) (4,31) (59,62) |
| 114 | (0,2) (0,111) (1,42) (3,98) (7,110) (33,99) (41,113) |

*Lower bounds.* Describe a chord set by its *type* (the cyclic pattern of chord
endpoints, of which there are 1,236 for five chords and 21,878 for six) and the
lengths of the arcs between endpoints. Every cycle length is then a fixed linear
form in the arc lengths, and "pancyclic" becomes a covering condition that does
not mention $n$. Searching all types:

| chords | largest $n$ found | search |
|---:|---:|---|
| 5 | 40 | 17,378 nodes for "$n\ge 41$"; confirmed by a second solver, by CP-SAT, and by the per-$n$ searcher at $n=41,42,43,44$ |
| 6 | 67 | 0.94 million nodes for "$n\ge 68$"; a second solver is still running the same question |
| 7 | 114 | 409 million nodes for "$n\ge 115$"; one program only |

Method, checks and their limits: `arcform/README.md`.

## What the data says about the open question

Erdős asked whether $h(n)\ge\log_2 n+\log_* n-O(1)$, and remarked he could not
even prove $h(n)-\log_2 n\to\infty$. Let $N(k)$ be the largest $n$ with
$h(n)\le k$, i.e. the largest $n$ for which a pancyclic graph with $n+k$ edges
exists. (Monotonicity of $h$ is not known in general; for $n\le 114$ it holds by
the table above.)

| $k$ | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| $N(k)$ | 3 | 5 | 8 | 14 | 24 | 40 | 67 | 114 |
| Bondy's counting bound $2^{k+1}+1$ | 3 | 5 | 9 | 17 | 33 | 65 | 129 | 257 |
| most different cycle lengths any type can have, plus 2 | 3 | 5 | 9 | 17 | 31 | 58 | 111 | 215 |
| $N(k)/2^k$ | 3 | 2.5 | 2 | 1.75 | 1.5 | 1.25 | 1.05 | 0.89 |
| $N(k)/N(k-1)$ | | 1.67 | 1.60 | 1.75 | 1.71 | 1.67 | 1.68 | 1.70 |
| extremal graphs up to symmetry | 1 | 1 | 3 | 6 | 2 | 5 | 3 | 2 |

All eight values are exact, with the caveat for $k=7$ stated above.

* $N(k)/2^k$ falls at every step. If it tends to 0 then $h(n)-\log_2 n\to\infty$,
  the statement Erdős could not prove. Eight values are evidence, not a proof.
* Counting cycles cannot explain the fall. The third row uses the true maximum
  number of cycle forms of any type instead of $2^{k+1}-1$ and is still far above
  $N(k)$: the obstruction is in the lengths, not in the number of cycles.
* The fall is slowing. $N(k)=2^{k-2}(10-k)$ fits $k=2..5$ exactly and predicts 64
  and 96 for $k=6,7$; the true values are 67 and 114. Each step multiplies $N$ by
  about 1.7. Since $h(n)\le\log_2 n+\log_* n+O(1)$ is known, the factor must
  come arbitrarily close to 2 on average eventually; the data are nowhere near
  that regime.
* A curiosity: the integers that $a(n)$ skips are $N(k)+k+1$ = 4, 7, 11, 18, 29,
  46, 74, 122. The first five are the Lucas numbers; the next three are 1, 2 and
  1 below the Lucas numbers 47, 76, 123. I have no explanation and do not claim
  one exists.
