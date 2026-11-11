# Handoff: state of the Erdős-frontier computations

Written 2026-11-11 so that work survives container replacement (the working
container was replaced at least three times; anything uncommitted was lost each
time, including one source file and every in-progress run).

## What exists and is pushed

Branch `claude/open-math-problems-47rwoa`. Start from `README.md` (top level) for
the full picture; `SUBMISSIONS.md` has the ready-to-file OEIS text;
`erdos357/certificates.json` and `erdos1016/certificates.json` hold every new
term with its witness. Published summary page source: `erdos-frontiers.html`
(also published as an artifact; URL was
`https://claude.ai/code/artifact/60ae5b68-55b4-4694-9b2b-ac5c331fb419`, private
to the owner; the artifact tools were disconnected at the time of writing, so
the repo copy is the source of truth).

New exact terms: A105206 n=23..40 (18 terms), A364132 k=23..26 (4), A364153
k=14..16 (3).

Nothing has been submitted to the OEIS. That needs the owner's account.

## Open item 1: decide h(41) (pins down N(5))

`h(40)=5` gives only `N(5) >= 40`, so the ratio `N(k)/2^k` is known exactly only
for `k <= 4`. Deciding whether a pancyclic `C_41` plus 5 chords exists settles
the next step. If none exists, `N(5)=40`; if one does, move on to `n=42`.

* Command: `cd erdos1016 && ./refute.sh 41 5 4` (86 tasks, 4 workers). It uses
  `pancyclic2`. Estimated 6.5e10 nodes, about 22 core-hours. Never completed.
* `pancyclic3.c` (committed) adds a mid-batch look-ahead prune and mirror
  symmetry. Measured wall-clock on refutations, against the previous searcher
  with rotation symmetry only: n=25,k=4: 14.4 s -> 2.1 s; n=30,k=4: 12.2 s ->
  0.9 s. If that factor holds at n=41 the decision drops to a few core-hours.
* **pancyclic3 is NOT yet validated for use in a proof.** `validate3.sh` (partial
  log: `erdos1016/validate3.txt`) got as
  far as: all 20 published terms reproduced, four refutations (n=25,26,30,33 with
  k=4) still IMPOSSIBLE, witness at n=35 found. Not yet done: witnesses at
  n=36,39,40, and the speed test at n=41 span 16.
* Before trusting a v3 *refutation* at n=41, do this: (a) add an enumerate-all
  mode and check that the set of solutions modulo the full dihedral group is the
  same under v2 (rotation only) and v3 (rotation + mirror) for every n <= 22 at
  k = h(n); (b) compare v2 and v3 verdicts on the n=41 spans that v2 already
  finished (16, 18, 20 are IMPOSSIBLE under v2). A wrong symmetry argument would
  produce a false "impossible", and this is a negative claim, so it needs the
  stronger check.

## Open item 2: A364132 / A364153 beyond the current frontier

`erdos357/psidon.sh` splits the Golomb-ruler search over the first gap
(validated to partition exactly). A364132(27) and A364153(17) are the next
terms; each is a multi-hour computation on 4 cores.

## Corrections already made (do not regress)

`h(40)=5` gives `N(5) >= 40`, a lower bound. An earlier draft read it as
evidence that `N(k)/2^k` keeps falling at `k=5`; it is not. The repo and the
page now state the exact values only for `k <= 4`.

## Lessons for running long jobs in this environment

* Commit and push every source file the moment it works. A file that exists
  only in the container is lost at the next replacement.
* Never rely on a background job surviving a turn boundary. Make runs resumable
  (slice the work, write one result line per slice, skip finished slices) and
  keep the result log in git.
* Verify the remote, not the local state: compare `git rev-parse HEAD` against
  `git ls-remote origin <branch>`.
