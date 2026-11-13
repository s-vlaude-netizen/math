# Handoff: state of the Erdős-frontier computations

First written 2026-11-11, rewritten 2026-11-13, so that work survives container
replacement (the working container was replaced several times; anything
uncommitted was lost each time).

## What exists and is pushed

Branch `claude/open-math-problems-47rwoa`. Start from `README.md` (top level) for
the full picture; `SUBMISSIONS.md` has the ready-to-file OEIS text;
`erdos357/certificates.json` and `erdos1016/certificates.json` hold every new
term with its witness. Summary page source: `erdos-frontiers.html` (also
published as a private artifact,
`https://claude.ai/code/artifact/60ae5b68-55b4-4694-9b2b-ac5c331fb419`).

New exact terms: A105206 n = 23..114 (92 terms), A364132 k = 23..26 (4), A364153
k = 14..16 (3).

Nothing has been submitted to the OEIS. The owner has no account yet;
`SUBMISSIONS.md` says where to register.

## Erdős 1016: where it stands (2026-11-13)

Open item 1 of the first handoff (decide h(41)) is closed, in two ways.

1. Per-n search with the gate-validated searcher (`p2-gates/pcsearch.c`,
   `decide.sh`): no pancyclic C_n plus 5 chords for n = 41, 42, 43, 44. About 20
   core-minutes per n, and no end in sight.
2. `erdos1016/arcform/`: a search that does not fix n. It reproduces the above
   and settles every n at once: the largest n with k chords is 5, 8, 14, 24, 40,
   67, 114 for k = 1..7. Read `erdos1016/arcform/README.md` first; it lists what
   was checked how.

Not finished:

* The second solver (`arcdfs`, version 1) is re-deriving "no 6 chords for
  n >= 68" in `arcform/xcheck/v1_k6_min68/`: resumable, about 12 core-hours in
  total. Restart with `cd erdos1016/arcform && ./runchunks.sh ./arcdfs 6 68 400 2 xcheck/v1_k6_min68`
  (needs `types_6.txt`: `./gentypes 6 > types_6.txt`, or `python3 arcform.py dump 6`
  for the order the finished parts used; the set of types is the same). When all
  400 parts are present and contain no line starting with `S`, the k = 6 row is
  confirmed by two solvers.
* k = 7 ("no 7 chords for n >= 115", hence N(7) = 114 and h(n) >= 8 from 115 on)
  comes from `arcdfs2` alone. Version 1 is far too slow for it. A second
  implementation of the two-ended search, or a proof log with a separate
  checker, would close this.
* k = 8: 13,550,514 types, about 4.6 million of them with enough cycles for
  n >= 193. Extrapolating from k = 7 this is weeks of core time with the present
  program. Witnesses with 8 chords for n >= 115 can be found by sampling types
  (`./gentypes 8 --minF F --part i m | ./arcdfs2 NMIN --max NMAX --print`).

## Open item 2: A364132 / A364153 beyond the current frontier

`erdos357/psidon.sh` splits the Golomb-ruler search over the first gap
(validated to partition exactly). A364132(27) and A364153(17) are the next
terms; each is a multi-hour computation on 4 cores.

## Corrections already made (do not regress)

`h(40)=5` alone gives only `N(5) >= 40`. An earlier draft read it as evidence
that `N(k)/2^k` keeps falling at `k=5`. That inference was wrong at the time; the
value `N(5)=40` is now established by the searches above.

`N(k)` is defined as the largest n for which a pancyclic graph with n+k edges
exists. Monotonicity of h is not known in general; it holds for n <= 114 by the
computed table.

## Lessons for running long jobs in this environment

* Commit and push every source file the moment it works. A file that exists
  only in the container is lost at the next replacement.
* Never rely on a background job surviving a turn boundary. Make runs resumable
  (slice the work, write one result line per slice, skip finished slices) and
  keep the result log in git.
* Verify the remote, not the local state: compare `git rev-parse HEAD` against
  `git ls-remote origin <branch>`.
* A pipeline's exit status is that of its last command. `a | b | grep ... > f && mv f done`
  marks a killed run as finished. Use `set -o pipefail`, require the program's own
  summary line, and compare the amount of work done with the amount expected.
* A background command started through the tool is stopped after 30 minutes.
  Long runs go through `nohup ... &` and must be resumable.
