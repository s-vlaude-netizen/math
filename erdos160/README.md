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
* `walkw4ap.c` — the same local search with constraint weighting (breakout / PAWS style: when no
  move improves, raise the weight of every currently violated progression). Far better than
  plain WalkSAT: N = 53 with 6 colours in about 1e5 iterations on a lucky seed, against several
  minutes for `walk4ap`. Run times are heavy-tailed (N = 50: 0.02 s to 15 s over three seeds), so
  many short runs with different seeds beat one long run.
* `color4ap_r.c` — the DFS with random restarts (not better than the local search here).

## The exact solver for k = 6 (`col6b.c`) and how far it is trusted

* `est4ap.c` estimates the DFS tree size with Knuth's random-probe estimator. It is accurate where
  it can be checked (k = 5, N = 36: estimate 3.07e5 +- 4e3, true 307,363 nodes). For k = 6 it gives
  about 4e9 nodes for every N from 46 to 62, i.e. the proof of "no colouring" costs the same wherever
  it is attempted above the threshold, about 5 core-hours at 215,000 nodes/s.
* `col6b.c` is the same search made deterministic so that it can be split over workers (the
  nodes at depth D are tasks, worker r of m takes tasks with index % m == r) and checkpointed
  (finished tasks are appended to a file; a restart skips them).
* **Checks of the solver:** it reproduces the known N_3 = 12, N_4 = 22, N_5 = 35 (a colouring at N and
  none at N + 1); on k = 5, N = 36 it uses exactly the 307,363 nodes of the unoptimised `color4ap.c`;
  three workers' node counts add up to that plus the shared top levels; a resumed run skips every
  finished task. `test_prop.c` tests the propagation rules against exhaustive completion on random partial
  assignments (k = 3, 4, 5, N = 11..13; about 170,000 colour removals checked, none of which a
  valid completion uses; propagation never failed when a completion existed).
* What is **not** independent: the claim "no 6-colouring of [1..N]" rests on this one program (no
  certificate); the soundness test and the reproduction of N_3..N_5 are the evidence for it.

## Findings

* **N_6 = 53.** The exhaustive search (`col6b`, k = 6, N = 54) finished: no 6-colouring of {1..54}
  (4.25e9 nodes, 43,036 tasks on 4 cores, about 2 hours; see `results.md` for the checks). So h(n) = 7 for
  n >= 54 up to the largest 7-colourable n. (An earlier attempt with the plain DFS did not finish; the
  cost estimate that made the parallel run worthwhile is in the section above.)
* A 6-colouring of `{1..53}` was found by local search and verified (matches the OEIS entry).
  See `results.md` for the longest verified 6-colourings found.
* Terms `h(n) = 6` for `36 <= n <= L` are exact once a 6-colouring of `[1..L]` is exhibited,
  because `N_5 = 35` already forces `h(n) >= 6` for `n >= 36`.
