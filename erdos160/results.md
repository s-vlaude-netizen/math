# Verified 6-colourings of {1..N} (every 4-term AP has >= 3 colours)

Checked by `verify.py`, independent of the solvers.

## N = 53 (found by walkw4ap, seed 1)

`4 1 4 3 2 6 1 2 6 3 3 5 2 5 1 4 3 6 6 4 1 2 4 2 1 1 5 3 5 2 6 1 3 3 6 4 5 6 2 1 4 3 4 5 3 5 6 4 2 6 1 5 2 `

## N_6 = 53: there is no 6-colouring of {1..54}

Exhaustive search (`col6b`, k = 6, N = 54, split depth 9, 4 workers; `col6_54_w*.out`, `col6_54_w*.ckpt`):
all 43,036 tasks finished, none produced a colouring. Together with the verified 6-colouring of {1..53}
above, **N_6 = 53**, hence h(n) = 6 for 36 <= n <= 53 and h(n) >= 7 for every n >= 54.

* Coverage: every task index 0..43035 appears exactly once across the four checkpoint files, and worker r holds
  exactly the indices with index mod 4 = r (checked by script).
* Size: the four workers visited 4,253,480,380 nodes in total; the random-probe estimator (`est4ap`, run before the
  search) predicted 4.25e9 +- 0.23e9.
* What this rests on: one program, no certificate. The program reproduces the known N_3 = 12, N_4 = 22, N_5 = 35,
  matches the unoptimised solver node for node on k = 5, N = 36 (307,363 nodes), and its propagation was tested
  against exhaustive completion on random partial assignments (no unsound removal in ~170,000 checked).
  It has not been reproduced by a second, independently written solver.
