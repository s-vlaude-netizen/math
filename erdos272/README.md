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
with a common element Szabó's formula is exactly the maximum for 11 <= n <= 16, and is a lower bound with the
centre as the (checked) best element for 17 and 18. This gives `a(n) >= formula` (known) and shows that no
improvement can come from a star at those n; any improvement would have to be a family with no common element.

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

Pending at the time of writing. A fixed-size-stack bug in an earlier build (frames of 128 KB,
overflow at recursion depth 64) was found and fixed before any result was taken from it.

Two things tried for the full graph that did **not** help (n = 10, bound 49, 461,344 nodes
in the natural order): other initial vertex orders (centre-containing sets first: 2x worse;
small sets first: 40x worse; by degree: 12x worse) and cutting a branch whose partial family
still has a common element that no remaining candidate avoids (never fires; the colouring bound
already disposes of star-like regions).
