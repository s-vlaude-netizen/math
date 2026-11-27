# Handoff (kept current; last updated 2026-11-27)

Canonical session: `session_012fG6WaCYEpznhJh1aAQAFu`, branch `ccr-dc987561-gkuljz` (see `SESSIONS.md`).
Nothing below has been submitted to the OEIS; submitting needs the owner's account.

## Results, in order of how ready they are

| Sequence | Status | Where |
|---|---|---|
| A389839 (Erdős 854) | a(13)..a(24) computed here and equal to Ziller's published table (arXiv:2007.01808); a(25)..a(44) are transcribed from that table. Draft ready. Prior work exists, so it is an entry extension, not a new result. | `SUBMISSIONS.md`, `erdos854/` |
| A402186 (Erdős 160) | **N_6 = 53**: exhaustive search shows no 6-colouring of {1..54} (4.25e9 nodes, one program, no certificate). 7-colourings verified up to N = see `erdos160/found_7_*.out`. So h(n) = 7 for 54 <= n <= largest N found. Draft to be written when the ladder stops. | `erdos160/` |
| A401861 (Erdős 272) | a(13) >= 82. Whether 83 sets are possible is being decided (`erdos272/n13b_*`). Exact side results (star families, non-star families) are comment drafts. | `erdos272/`, `SUBMISSIONS.md` |
| A399720 (Erdős 817) | a(7) not reachable with the current search. | `erdos817/STATUS.md` |
| A105206 (Erdős 1016) | k = 7 row (N(7) = 114) still not reproduced by a second search program; the other session's partial work is merged. | `erdos1016/`, `SESSIONS.md` |

## Jobs that may be running and how to resume them (the container can restart at any time; checkpoints are committed)

* **A401861 n = 13, bound 83** (4 workers). From `erdos272/`:
  `gcc -O3 -march=native -o clique5 clique5.c`, then for r in 0..3:
  `SPLIT0=1 S1M=4 S1R=$r CKPT=n13b_w$r.ckpt CKPT1=n13b_w$r.ckpt1 CKPTR=n13_lb83_part0.ckpt:n13_lb83_part1.ckpt:n13_lb83_part2.ckpt:n13_lb83_root0.ckpt ./clique5 13 83 4 $r > n13b_w$r.out 2> n13b_w$r.err &`
  A worker that finishes prints `n=13 none >= 83 ...`; a clique of size >= 83 would be printed instead. The answer is
  "none >= 83" only when all four workers have printed it.
* **A402186 7-colourings**: `cd erdos160; ./ladder7.sh 7 <N> 95 40 25 <seed_offset>` (several copies with different offsets race on the same N).

## Constraints met in this environment

No scipy, sympy, mpmath, SAT solver or network installs (installs were refused), so the Kelvin experiments in
`quantumalgorithms` could not be run; everything else uses C and numpy only.
