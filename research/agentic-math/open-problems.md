# Open problems in agentic mathematical research, and first ideas

Written 2026-11-11 after literature round 1 (`literature-round1.md`). Two kinds of
statement are kept apart on purpose:

* **Evidence** is what a paper says, with its arXiv id.
* **Gap** and **Idea** are my inference. None of the ideas has been checked
  against prior work yet, so none is claimed to be new.

Why these problems at all: the Erdős-frontier computations in this repository
(`handoff.md`) are a small testbed. They have exact certificates, an independent
verifier, a published reference to reproduce before extending, and a record of
mistakes that were caught and one that was not (below). That makes them a cheap
place to test ideas that the literature only argues about.

## P1. The intent gap: "technically correct" is not "correct"

* **Evidence.** 31.5% of checked candidates were correct under some reading, 6.5%
  under the intended reading (2602.10177). Technically correct solutions mostly
  came from misinterpreted statements (2601.22401). Proofs of weaker versions
  (2610.09769). Silent problem reformulation as a named failure mode (2606.24902).
  A Lean-certified result disputed over what the formalization meant (2608.28997).
* **Gap.** No cheap, automatic test that a solution addresses the statement the
  problem-setter meant. The formal side is crowded (2610.20573, 2610.25018,
  2610.32919, 2610.18498); the informal side looks thinner, but that is unchecked.
* **Idea.** A *statement contract* fixed before the work starts: the exact claim, a
  few instances with known answers, and the easier misreadings listed as traps. A
  separate checker scores the result against it.
* **First experiment.** On the Erdős problems with formal statements, see how often
  a "solution" survives the contract. Needs model calls, so it is the most
  expensive item here.

## P2. Soundness of agent-written pruning and symmetry breaking

* **Evidence.** Verifiers can confirm that a candidate holds but cannot prove that
  none was missed (2608.19009, the completeness blind spot). Every negative result
  in this repository ("no pancyclic C_n plus k chords exists") is exactly that
  kind of claim.
* **Gap.** I found nothing in round 1 on how to certify a *non-existence* result
  produced by search code an agent wrote, where the likeliest bug is a symmetry
  reduction that silently discards solutions.
* **Own data point (n = 1, anecdotal).** The first rotation-symmetry version of the
  pancyclic searcher gave wrong values at the k = 0 edge case. It was caught only
  because its output was compared with the 20 published terms. A second edge case
  (k = 1 in a tie-break) was found by re-reading, not by a test.
* **Idea.** Mutation-test the gates. Take a correct searcher, inject plausible
  unsound mutations (strict instead of non-strict comparison in a symmetry
  tie-break, a mirror map off by one, a counting bound one too tight, a skipped
  first or last candidate), and measure which gate kills which mutant:
  (G1) reproduce the published terms; (G2) *orbit-differential*: enumerate all
  solutions of a naive and an optimized searcher on small instances and compare
  the sets modulo the full symmetry group; (G3) random instances against brute
  force; (G4) re-check witnesses independently. Hypothesis: G1 misses mutants whose
  damage lies beyond the published range, and G2 catches almost all of them cheaply.
* **First experiment.** Cheap, CPU only, and it has real stakes: the same G2 check
  is what `handoff.md` requires before the faster searcher may be used to decide
  h(41).
* **Prior-art risk.** Symmetry-breaking correctness is an old topic in SAT and
  constraint solving, and mutation testing is standard software engineering. What
  is unchecked is whether anyone measured it for agent-written search code.

## P3. Auditable novelty: how do you know a result is new?

* **Evidence.** Determining whether a solution already existed was harder for the
  experts than verifying it (2601.22401 §1.6). Four earlier AI claims were later
  found to overlap the literature. "Subconscious plagiarism" is named as a new
  risk, and the authors call their own novelty classification an upper bound.
* **Gap.** "No prior work found" is reported with no estimate of how likely the
  search was to find it.
* **Idea.** Calibrate the search with *planted rediscoveries*. The Erdős case study
  already lists a dozen problems that were marked open while a solution existed
  (333, 591, 705, 992, 1105, 397, 659, 935, 1089, and others). Hide the answer,
  run the literature agent on the bare statement, record recall. A novelty claim is
  then reported as "no hit, with recall r on a comparable set".
* **Weak spot in my own results.** My novelty claims for the new OEIS terms rest on
  one check: the live OEIS entries and b-files as of 2026-09-04. That is a small
  search, and the notes in this repository should say so wherever novelty is
  mentioned.

## P4. Reformulation as a measurable skill

* **Evidence.** Station's reasoning agents lost to evolutionary search on irregular
  constructions (2608.23691), while search alone does not produce the equivalence
  arguments that made my two problems finite. Silent reformulation is a failure
  mode when unproved (2606.24902).
* **Gap and idea (mine).** In this repository the decisive step was an *equivalence
  proof*, not search: all consecutive sums distinct if and only if the partial sums
  form a Golomb ruler; a pancyclic graph is Hamiltonian, so it is a cycle plus
  chords with a cycle space of dimension k + 1. A benchmark could ask an agent, for
  an OEIS sequence marked "more", for a faster algorithm with (a) the published
  terms reproduced, (b) an equivalence argument, (c) new verified terms. Score:
  new verified terms per CPU hour against the best published program.
* **First experiment.** Run it by hand on the roughly 20 shortlisted sequences from
  `survey/` and record how many admit a reformulation at all.

## P5. Which problems are reachable?

* **Evidence.** "Open" because of obscurity, not difficulty (2601.22401). LANTERN
  mines OEIS for connections (2609.32264).
* **Idea.** A retrospective test. The community database has status and update
  dates for 1217 problems, and a wiki lists AI contributions. Train on changes up
  to a cutoff, predict later ones from metadata (tags, OEIS link, prize,
  formalization state), and compare with random.
* **Risks.** Few positives, and human and AI status changes are confounded.
* **Not yet done.** The wiki page has not been fetched.

## P6. A reporting card for AI-produced results

* **Evidence.** Aletheia proposes autonomy levels A, C and H plus a significance
  axis; VAL grades verifiers L0–L5; two papers argue that cheap verification moves
  the bottleneck to adjudication (2608.28997, 2611.12943).
* **Own data point.** A claim of mine was wrong: I wrote that N(k)/2^k keeps falling
  at k = 5, but h(40) = 5 only gives N(5) at least 40, a *lower* bound, which does
  not continue the trend. It was caught only while drawing a figure that forced the
  range into view.
* **Idea.** A small machine-readable card per result: exact statement; kind (exact,
  lower bound, upper bound, conjecture); witness or certificate; verification
  level; reproduction gate (published values reproduced, n of n); independent
  checks; novelty scope with date and recall; autonomy level; compute; caveats. A
  validator rejects an "exact" claim without a non-existence argument, and a
  derived claim inherits the weakest bound type of its inputs.
* **First experiment.** Fill cards for the 25 new terms and see which fields are
  empty or contested. That is cheap and shows whether the schema is worth anything.

## P7. Durable state for long runs

* **Evidence.** Bolzano keeps a persistent research record; Stellar Colosseum routes
  findings back to the affected part (2610.09769, 2609.15983).
* **Own data point.** The working container was replaced at least three times.
  A source file and every in-progress run were lost.
* **Idea.** Commit-before-continue, plus runs cut into resumable slices with one
  result line per slice. Mostly engineering; low priority unless the failure rate
  turns out to matter for results.

## Suggested order

1. **P2**, because it is cheap, falls out of work already pending, and has a
   measurable outcome (mutant kill rates).
2. **P6**, because it is cheap and tests a schema on data I already have.
3. **P5**, a data analysis with no model calls, once the wiki is fetched.

**Round 2 should begin with a prior-art check** for P2, P5 and P6 (symmetry-breaking
soundness testing, reachability prediction, result schemas), then run the P2 gates
on the two searchers here. Rough cost: a few CPU hours and a modest number of
searches; no large model usage.
