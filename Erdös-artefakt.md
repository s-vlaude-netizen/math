Computational note · September 2026

# Two Erdős Frontiers

Neither conjecture is settled. What moved is the exact data behind them — because each problem has a reformulation that turns an unbounded search into a finite one.

Erdős #357 Erdős #1016 A105206 A364132 A364153

25 new exact terms, none previously published

3 OEIS sequences extended, all published terms reproduced first

0 conjectures resolved — both problems remain open

Choosing a target

## Where is the frontier actually reachable?

Most open problems are open because no finite computation touches them. The useful question is narrower: which open problem has a next value that nobody has computed?

That question can be answered mechanically. The community database of Erdős problems was cross-referenced against the OEIS, and the sequences carrying the editors' more keyword — “further terms wanted” — were sorted by how few terms are known. A short list of frontiers falls out.

1217

problems in the database

604

still open

214

open *and* carrying an OEIS sequence

2

picked, both because a reformulation collapses the search

#357 A364132 · A364153

## Sequences with distinct consecutive sums

> Let 1 ≤ a₁ \< ⋯ \< ak ≤ n be integers such that all sums of the shape au + ⋯ + av are distinct. Let f(n) be the maximal such k. How does f(n) grow? Is f(n) = o(n)?
> 
> Erdős and Harzheim — still open

Searching tuples directly is hopeless. But pass to the partial sums P₀ = 0, Pi = a₁ + ⋯ + ai, and a consecutive sum becomes a difference Pv − Pu−1.

All consecutive sums are distinct exactly when the partial sums form a **Sidon set** — a Golomb ruler. The side condition ai ≤ n says every gap of that ruler is at most n. So both sequences ask one thing: **minimise the largest gap of a Golomb ruler with k+1 marks.**

The witness for a(6) = 7: gaps 2, 1, 7, 6, 5, 4. Its 21 consecutive sums *are* the 21 pairwise differences of the seven marks, and they are distinct — so the marks are a Sidon set, and the largest gap, 7, is the quantity being minimised.

### New terms

For each, every smaller bound was *refuted* by exhaustive search before the witness was accepted; the node counts show how much search that took.

**A364153** — arbitrary order (inverse of g). Published: 13 terms, ending a(13) = 18.

| k | a(k) | witness (a₁, …, ak) | refuted below |
| --- | --- | --- | --- |
| 14 | 20 | 1, 4, 6, 17, 15, 14, 20, 16, 19, 18, 8, 13, 9, 3 | m = 18, 19 (149 445 063 nodes) |
| 15 | 22 | 1, 11, 16, 8, 21, 22, 20, 19, 15, 18, 14, 17, 6, 3, 4 | m = 20, 21 (1 099 325 340) |
| 16 | 24 | 1, 2, 6, 5, 23, 20, 24, 22, 18, 12, 21, 17, 15, 10, 16, 19 | m = 22, 23 (7 920 625 479) |

**A364132** — Erdős's original monotone case (inverse of f). Published: 22 terms, ending a(22) = 52.

| k | a(k) | witness (a₁ \< ⋯ \< ak) | refuted below |
| --- | --- | --- | --- |
| 23 | 56 | 1, 4, 6, 8, 24, 27, 33, 35, 36, 37, 39, 40, 41, 44, 45, 46, 48, 49, 52, 53, 54, 55, 56 | m = 52…55 (566 123 991) |
| 24 | 60 | 1, 6, 12, 15, 30, 31, 36, 37, 40, 41, 42, 43, 44, 46, 47, 49, 50, 53, 54, 55, 56, 58, 59, 60 | m = 56…59 (2 193 364 619) |
| 25 | 63 | 2, 4, 8, 10, 13, 33, 34, 41, 42, 43, 44, 45, 47, 48, 49, 51, 52, 53, 54, 55, 57, 59, 61, 62, 63 | m = 60…62 (5 199 898 929) |
| 26 | 67 | 1, 2, 4, 5, 14, 22, 35, 39, 42, 44, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 61, 63, 64, 65, 66, 67 | m = 63…66 (18 859 978 541) |

Inverting gives the functions Erdős asked about. At the record points the ratio k / a(k) — which is f(n)/n where it is largest — keeps sliding: .500 at k = 12, .426 at 20, .411 at 23, .400 at 24, .388 at 26. Suggestive of f(n) = o(n), and no more than suggestive: the best proven lower bound in the monotone case is f(n) ≥ (2+o(1))√n, which at n = 67 would give 16 against the true value 26.

#1016 A105206

## How sparse can a pancyclic graph be?

> Let h(n) be minimal such that there is a graph on n vertices with n + h(n) edges containing a cycle on k vertices for every 3 ≤ k ≤ n. Estimate h(n).
> 
> Erdős — still open; he could not even prove that h(n) − log₂ n → ∞

Enumerating graphs is out of the question. Two observations cut it down to something a laptop finishes:

A pancyclic graph contains an n-cycle, so it is Hamiltonian — every candidate is Cn plus k chords. And its cycle space has dimension only k+1, so the graph has at most 2k+1 − 1 cycles *in total*, and its entire cycle spectrum is that many bitmasks — no cycle enumeration at all.

The second observation is also Bondy's lower bound, 2k+1 − 1 ≥ n − 2: too few chords cannot supply enough cycles, whatever you do with them.

| GF(2) combination | cycle length |
| --- | --- |
| H | 8 |
| c₁ | 3 |
| c₂ | 6 |
| H ⊕ c₁ | 7 |
| H ⊕ c₂ | 4 |
| c₁ ⊕ c₂ | 5 |
| H ⊕ c₁ ⊕ c₂ | not a cycle |

C₈ plus the chords (0,2) and (0,5) — ten edges, and pancyclic. Its cycle space has dimension 3, so seven combinations exist; six of them are genuine cycles and their lengths are exactly 3, 4, 5, 6, 7, 8. Nothing is spare, which is why n = 8 is the largest n that two chords can do.

### New terms

A105206 was known for n = 3…22. h(n) is now determined for n = 23…40 as well.

n 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40\
a(n) **27 28 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45**

Each row: a witness with h chords, plus a lower bound ruling out h−1. Chords are added to the cycle 0-1-⋯-(n−1)-0.

| n | h(n) | a(n) | witness chords | lower bound from |
| --- | --- | --- | --- | --- |
| 23 | 4 | 27 | (0,2) (0,20) (1,18) (8,20) | exhaustive search |
| 24 | 4 | 28 | (0,2) (0,21) (1,19) (8,23) | exhaustive search |
| 25 | 5 | 30 | (0,2) (0,3) (0,4) (0,20) (1,16) | exhaustive search |
| 26 | 5 | 31 | (0,2) (0,3) (0,4) (1,11) (1,22) | exhaustive search |
| 27 | 5 | 32 | (0,2) (0,3) (0,5) (1,9) (3,18) | exhaustive search |
| 28 | 5 | 33 | (0,2) (0,3) (0,5) (1,17) (2,21) | exhaustive search |
| 29 | 5 | 34 | (0,2) (0,3) (0,5) (1,18) (2,22) | exhaustive search |
| 30 | 5 | 35 | (0,2) (0,3) (0,9) (3,11) (4,16) | exhaustive search |
| 31 | 5 | 36 | (0,2) (0,3) (0,10) (5,9) (6,24) | exhaustive search |
| 32 | 5 | 37 | (0,2) (0,3) (0,28) (1,14) (6,31) | exhaustive search |
| 33 | 5 | 38 | (0,2) (0,3) (0,29) (1,14) (6,32) | exhaustive search |
| 34 | 5 | 39 | (0,2) (0,4) (0,21) (1,7) (3,26) | counting bound |
| 35 | 5 | 40 | (0,2) (0,10) (1,4) (3,18) (6,34) | counting bound |
| 36 | 5 | 41 | (0,2) (0,11) (1,7) (7,10) (8,19) | counting bound |
| 37 | 5 | 42 | (9,26) (14,24) (25,28) (9,22) (23,25) | counting bound |
| 38 | 5 | 43 | (1,24) (0,35) (24,26) (25,37) (19,36) | counting bound |
| 39 | 5 | 44 | (13,31) (11,14) (12,28) (19,30) (29,31) | counting bound |
| 40 | 5 | 45 | (20,29) (28,31) (10,25) (26,28) (27,33) | counting bound |

Up to n = 33 the lower bound had to be earned: every chord set one size smaller was refuted, up to rotation. The hardest was n = 25 with four chords — 233 829 950 nodes without the symmetry reduction, 10 997 097 with it. From n = 34 on it is free, because 2⁵ − 1 = 31 \< n − 2: four chords simply cannot make enough cycles, and only a witness is needed.

What the data says

## Evidence on Erdős's own question

Write N(k) for the largest n with h(n) = k — the point where h falls furthest below log₂ n. If N(k)/2k keeps falling, then h(n) − log₂ n → ∞, which is what Erdős believed and could not prove.

Measured and exact for k ≤ 4; falling at every one of those four steps. For k = 5 only the range is known, and the fan shows both ways it can go.

| k | 0 | 1 | 2 | 3 | 4 | 5 |
| --- | --- | --- | --- | --- | --- | --- |
| N(k) | 3 | 5 | 8 | 14 | 24 | \[40, 65\] |
| Bondy's counting bound 2k+1+1 | 3 | 5 | 9 | 17 | 33 | 65 |
| N(k) / 2k | 3 | 2.5 | 2 | 1.75 | 1.5 | \[1.25, 2.03\] |

### The caveat, stated plainly

h(40) = 5 gives only N(5) ≥ 40, that is N(5)/2⁵ ≥ 1.25 — a *lower* bound, which on its own does not continue the fall. The fall continues if and only if N(5) \< 48. Deciding it means deciding h(41): if no five-chord witness exists at n = 41, then N(5) = 40 and the ratio drops to 1.25; if one does, the question moves to n = 42. That is a finite computation — roughly 6.5 × 10¹⁰ search nodes, about 22 core-hours — and it was started but not finished here.

Verification

## Why the numbers should be believed

A new term is only as good as the program that produced it, so nothing here rests on a single program.

- **Both searchers reproduce every published term first.** All 20 known values of A105206, all 22 of A364132, all 13 of A364153 — before any new value was accepted.
- **The pancyclic witnesses are re-checked without the cycle-space argument at all**, by enumerating every simple cycle with networkx. The two spectrum computations were also compared on 4000 random graphs: no mismatch.
- **The Golomb witnesses are re-expanded** into all k(k+1)/2 consecutive sums, which are then tested for distinctness.
- **The symmetry reduction was checked against the unreduced search.** At n = 25: 233 829 950 nodes versus 10 997 097, same verdict. At n = 26: 311 112 744 versus 13 906 189, same verdict.
- **Work-splitting was checked to partition exactly.** Four slices of one branch sum to the unsliced node count, to the three repeated root entries.

Reproduce it

## Everything is one build away

```
gcc -O3 -march=native -o sidon sidon.c
./runseq.sh 16 0 1          # A364153(16), from scratch
python3 verify_all.py       # re-derive every claimed term

gcc -O3 -march=native -o pancyclic pancyclic.c
./scan.sh 30 6              # h(30), witness + refutation
python3 verify_pancyclic.py crosscheck
```

Each new term is recorded in a certificates.json alongside its witness and the bounds that were refuted, and SUBMISSIONS.md holds the text ready to file with the OEIS.

Sources: [erdosproblems.com/357](https://www.erdosproblems.com/357) · [erdosproblems.com/1016](https://www.erdosproblems.com/1016) · [A105206](https://oeis.org/A105206) · [A364132](https://oeis.org/A364132) · [A364153](https://oeis.org/A364153) · [Erdős problem database](https://github.com/teorth/erdosproblems)

Both problems remain open. What changed is the table of values underneath them.
