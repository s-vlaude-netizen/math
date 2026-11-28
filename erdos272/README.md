# Erdős problem 272 — families of sets whose pairwise intersections are non-empty arithmetic progressions

[Problem 272](https://www.erdosproblems.com/272) · OEIS [A401861](https://oeis.org/A401861)

Let `a(n)` be the largest number of subsets of `{1..n}` such that every two have an
intersection that is a **non-empty arithmetic progression** (one or two elements count).
Simonovits–Sós asked whether `C(n,2)+1` is best possible; Szabó (1999) showed it is not,
giving `a(n) >= C(n,2) + floor((n-1)/4) + 1`, and conjectured that `a(n) = C(n,2) + O(n)` and
that extremal families have an element common to all sets.

The OEIS entry (new, Nov 2026) has `a(1..12) = 1, 2, 4, 7, 12, 17, 23, 30, 39, 48, 58, 69`.
All twelve equal Szabó's bound.

## Method

`clique.c` — exact maximum clique (bit-parallel branch and bound with greedy colouring) on the
graph whose vertices are the non-empty subsets of `{1..n}` and whose edges join two sets when
their intersection is a non-empty AP. Reproduces `a(1..10)` and shows no family of 49 sets
for n = 10, no family of 59 for n = 11.

`clique_star.c` — the same, restricted to sets that contain one fixed element `x` ("stars").
This graph is much easier: the colouring bound is already tight at the root, so the search
needs about as many nodes as the clique has vertices.

## Results

Maximum family when all sets contain the centre element `x = ceil(n/2)` (exact):

| n | star maximum | Szabó's formula |
|--:|--:|--:|
| 11 | 58 | 58 |
| 12 | 69 | 69 |
| 13 | 82 | 82 |
| 14 | 95 | 95 |
| 15 | 109 | 109 |
| 16 | 124 | 124 |
| 17 | 141 | 141 |
| 18 | 158 | 158 |

For every n from 11 to 16 all choices of the common element were run (x = 1 .. ceil(n/2); the rest follow by
reflection): the centre `x = ceil(n/2)` gives Szabó's value and every other position is strictly lower
(n = 13: 79, 79, 80, 80, 81, 81, 82 for x = 1..7). For n = 17 and 18 only the centre was run. So among families
with a common element Szabó's formula is exactly the maximum for 11 <= n <= 16. For n = 17 and 18 the centre
alone reaches the formula; the other positions were not run, so the maximum over all positions is not established
there. This gives `a(n) >= formula` (known) and shows that for 11 <= n <= 16 no improvement can come from a star;
any improvement would have to be a family with no common element.

## Families with no common element (exact, n <= 11)

`clique_nonstar.c` records only cliques whose sets have empty common intersection.

| n | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
|--:|--:|--:|--:|--:|--:|--:|--:|
| largest family with no common element | 11 | 16 | 22 | 29 | 37 | 46 | 56 |
| C(n,2)+1 | 11 | 16 | 22 | 29 | 37 | 46 | 56 |
| Szabó's formula (families with a common element) | 12 | 17 | 23 | 30 | 39 | 48 | 58 |

So for 5 <= n <= 11 the best family without a common element is exactly the old Simonovits–Sós
value `C(n,2)+1`, and the extremal one for n = 8, 9 is: every 2- and 3-subset containing a fixed
element `x` except `{x}` itself, together with the single set `{1..n} \ {x}` (its intersection
with each of those is a pair, hence an AP). This is the data point behind Szabó's conjecture
that extremal families have a common element, and it says more: the gap to the best star
family is `floor((n-1)/4)`, growing with n. It also means the n = 13 search below is looking for
something 4 sets above the best non-star family the pattern predicts (79).

## Status of the full n = 13 search

**Result (2026-11-28): no family of 83 sets exists for n = 13, so a(13) = 82 = Szabó's bound.**
`clique5.c` searched the full graph on the 8191 non-empty subsets of {1..13} for a clique of size 83 or more; the work was
split into four shares by root vertex (root 0, the largest, is split among them one level down), and all four printed
`n=13 none >= 83` (`n13b_w0.out` ... `n13b_w3.out`; 39.8M, 106.0M, 31.1M and 75.3M nodes in the last process of each share;
about 5e8 nodes and 6 CPU-hours over all partial runs, including the 3-way runs `n13_lb83_part*` whose finished roots were
imported). The lower bound is an explicit 82-family (`family_n13_82.txt`, found by `clique_star 13 82 1 0 7`, checked by
the independent script `verify_family.py`: all 3321 pairwise intersections are non-empty APs).

How much to trust it (one program, no replayable certificate):
* the program reproduces a(1..10) and finds no family of 59 sets at n = 11;
* at n = 11 the four shares are an exact partition of the search tree: with bound 59 their node counts sum to 4954058, the
  unsplit run has 4954052, and the difference 6 is exactly the repeated depth-0 and root-0 calls (3 + 3); with bound 58
  two of the four shares find 58-families;
* roots 27, 58 and 23 (one from each earlier 3-way run) were recomputed alone with the current program and gave exactly the
  node counts of the old logs (318144, 307756, 372659), so the imported "finished root" lists use the same numbering;
* `check_n13_coverage.py` confirms that every root of each share up to its last one (1328, 1329, 1330, 1331 for shares 0..3, after
  which the colouring bound stops the loop) and every one of the 861 children of root 0 appears in some finished list.

A fixed-size-stack bug in an earlier build (frames of 128 KB,
overflow at recursion depth 64) was found and fixed before any result was taken from it.

Two things tried for the full graph that did **not** help (n = 10, bound 49, 461,344 nodes
in the natural order): other initial vertex orders (centre-containing sets first: 2x worse;
small sets first: 40x worse; by degree: 12x worse) and cutting a branch whose partial family
still has a common element that no remaining candidate avoids (never fires; the colouring bound
already disposes of star-like regions).

## Is n = 14 feasible? (not attempted)

`est5.c` is a Knuth-style estimator of the number of search nodes of `clique5` (random root-to-leaf probes, stratified over the
root vertices). Calibration: at n = 11, bound 59 (true 4.95M nodes) it gave 3.1M-5.5M; at n = 13, bound 83 (true about 4.9e8
nodes) it gave 1.8e8 and 3.1e8 (standard errors 15% and 47%), so it reads low by a factor of about 1.6-2.8 and its error
bars are too small. For n = 14, bound 96 (Szabó's value is 95, and stars reach 95 for every centre) one run gave 1.3e10 nodes
with a standard error of 92% (`est5_n14_lb96_seed1.txt`; 95% of that estimate sits in the roots 10..99, i.e. in a few huge
probes). Correcting for the low reading, the full search needs roughly 1e10 to 1e11 nodes. With bitsets twice as wide as at
n = 13 (about 25 microseconds per node instead of 12) that is on the order of 100-300 CPU-hours, so it was not started. A
better bound (infra-chromatic or recolouring) or a symmetry argument would be needed to make a(14) reachable.
