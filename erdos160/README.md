# Erdős problem 160 — colourings with no 4-term AP using at most two colours

[Problem 160](https://www.erdosproblems.com/160) · OEIS [A402186](https://oeis.org/A402186)

`h(N)` = least `k` such that `{1..N}` has a `k`-colouring in which every 4-term arithmetic
progression contains at least three distinct colours. Let `N_k` be the largest `N` with a
`k`-colouring. A402186 (Manyama, Nov 2026) lists `h(1..53)`: `N_3 = 12`, `N_4 = 22`, `N_5 = 35`,
and `h = 6` from `n = 36` to the end of the data (`N_6 >= 53`).

## Tools

* `color4ap.c` — exhaustive DFS with forward checking and colour-symmetry breaking; decides
  "is there a k-colouring of [1..N]?". Reproduces `N_3 = 12`, `N_4 = 22`, `N_5 = 35`
  (N = 36, k = 5: no colouring, 307,363 nodes, 1.7 s).
* `walk4ap.c` — WalkSAT-style local search for colourings; `verify.py` checks a colouring
  independently of both solvers.
* `gen_cnf.py`, `gen_cnf2.py`, `solve_col*.sh` — CNF encodings for `../sat/sat.c`
  (the direct 4-literal encoding is far better).
* `color4ap_r.c` — the DFS with random restarts (not better than the local search here).

## Findings

* The exhaustive search for 6 colours is out of reach: a first-moment count of valid prefixes
  predicts about 5e9 DFS nodes, and N = 66 did not finish in 2 minutes. So `N_6` is not determined here.
* A 6-colouring of `{1..53}` was found by local search and verified (matches the OEIS entry).
  See `results.md` for the longest verified 6-colourings found.
* Terms `h(n) = 6` for `36 <= n <= L` are exact once a 6-colouring of `[1..L]` is exhibited,
  because `N_5 = 35` already forces `h(n) >= 6` for `n >= 36`.
