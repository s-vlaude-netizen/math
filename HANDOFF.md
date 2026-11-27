# Handoff (kept current; last updated 2026-11-27)

Canonical session: `session_012fG6WaCYEpznhJh1aAQAFu`, branch `ccr-dc987561-gkuljz` (see `SESSIONS.md`).
Nothing below has been submitted to the OEIS; submitting needs the owner's account.

## Results, in order of how ready they are

| Sequence | Status | Where |
|---|---|---|
| A389839 (Erdős 854) | a(13)..a(24) computed here and equal to Ziller's published table (arXiv:2007.01808); a(25)..a(44) are transcribed from that table. Draft ready. Prior work exists, so it is an entry extension, not a new result. | `SUBMISSIONS.md`, `erdos854/` |
| A402186 (Erdős 160) | **N_6 = 53**: exhaustive search shows no 6-colouring of {1..54} (4.25e9 nodes, one program, no certificate). 7-colourings verified up to N = 74 (`erdos160/found_7_*.out`; N = 75 not found in 80 seeds x 25 s). So h(n) = 7 for 54 <= n <= largest N found. Drafts written (`SUBMISSIONS.md`: terms 54..74, b-file `erdos160/b402186.txt`, companion sequence 3, 3, 12, 22, 35, 53). | `erdos160/` |
| A401861 (Erdős 272) | a(13) >= 82. Whether 83 sets are possible is being decided (`erdos272/n13b_*`). Exact side results (star families, non-star families) are comment drafts. | `erdos272/`, `SUBMISSIONS.md` |
| Kelvin, `quantumalgorithms` (Result 102) | Exact certificates: weighted 4-regular circulants beat the bipartite families of Result 100 for n = 10..17 (e.g. n = 17: -6.528e-3 against -2.197e-3). Not a proof of minimality. Written up in `RESEARCH_LOG.md`, `README.md`, `NEXT_STEPS.md`; tests in `tests/test_first_kind_numpy.py`. A floating-point false start (spurious -4.34e-3 at n = 14) was caught by the exact certifier and is documented. | `quantumalgorithms/experiments/exp050..054` |
| A399720 (Erdős 817) | a(7) not reachable with the current search. | `erdos817/STATUS.md` |
| A105206 (Erdős 1016) | k = 7 row (N(7) = 114) still not reproduced by a second search program; the other session's partial work is merged. | `erdos1016/`, `SESSIONS.md` |

## Jobs that may be running and how to resume them (the container can restart at any time; checkpoints are committed)

* **A401861 n = 13, bound 83** (4 workers). From `erdos272/`:
  `gcc -O3 -march=native -o clique5 clique5.c`, then for r in 0..3:
  `SPLIT0=1 S1M=4 S1R=$r CKPT=n13b_w$r.ckpt CKPT1=n13b_w$r.ckpt1 CKPTR=n13_lb83_part0.ckpt:n13_lb83_part1.ckpt:n13_lb83_part2.ckpt:n13_lb83_root0.ckpt ./clique5 13 83 4 $r > n13b_w$r.out 2> n13b_w$r.err &`
  A worker that finishes prints `n=13 none >= 83 ...`; a clique of size >= 83 would be printed instead. The answer is
  "none >= 83" only when all four workers have printed it.
* **Kelvin enumerations** (`quantumalgorithms/experiments/exp054_unweighted_circulants_exact.py --n N`, N = 18, 19, 20 may still be running; results in `results/exp054_n*.json`).
* **A402186 7-colourings**: `cd erdos160; ./ladder7.sh 7 <N> 95 40 25 <seed_offset>` (several copies with different offsets race on the same N).

## Constraints met in this environment

No scipy, sympy, mpmath, SAT solver or network installs (installs were refused), so the Kelvin experiments in
`quantumalgorithms` could not be run; everything else uses C and numpy only.
