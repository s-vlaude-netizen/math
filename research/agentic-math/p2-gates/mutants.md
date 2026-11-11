# Mutants of `pcsearch.c` — registered before any gate was run on them

Each mutant is one single-site edit of the searcher, built with `-DMUT=<id>`. The
edits are the standard mutation operators (relational operator replacement,
off-by-one on a constant or bound, statement deletion, wrong variable or wrong
level) applied to every pruning, ordering, symmetry and bookkeeping site of the
program, plus one bug the first version of the searcher really had (38) and two
bugs that can only show at large sizes (46, 47).

This file, the program and the gate runner were committed before the mutants were
run, so the list could not be tuned to the outcome. The "prediction" column is my
guess, written down in advance, of whether the edit changes any verdict:
**U** = unsound (loses solutions or accepts wrong ones), **S** = sound (at worst
slower or with redundant output), **A** = should trip the program's own assertion.

| id | site | edit | prediction |
|---:|---|---|:---:|
| 1 | spectrum | minimum cycle length 3 → 4 | U |
| 2 | spectrum | connectivity walk skipped (a union of cycles counts as one cycle) | U |
| 3 | spectrum | fundamental cycle takes one path edge too many | U |
| 4 | spectrum | closing edge of the Hamiltonian cycle has wrong endpoints | U |
| 5 | bookkeeping | largest length considered N → N−1 | S (length N is always present) |
| 7 | bookkeeping | lengths not undone when the look-ahead cuts a branch | U |
| 8 | count prune | `missing > B` → `missing >= B` | U |
| 9 | count prune | total 2^(K+1) → 2^K | U |
| 10 | count prune | level term 2^(j+1) → 2^(j+2) | U |
| 11 | look-ahead | `>` → `>=` | U |
| 12 | look-ahead | remaining elements in the batch off by one | U |
| 13 | look-ahead | elements from later chords ignored | U |
| 14 | rotation | span loop `<= N/2` → `< N/2` | U |
| 15 | rotation | candidates `span >= s` → `span > s` | U |
| 16 | rotation | span computed without wrap-around | S (candidate list only grows) |
| 17 | rotation | forced chord not excluded from the candidates | S for verdicts; emits sets with a repeated chord |
| 18 | mirror | rule at the first free chord `<` → `<=` | U |
| 19 | mirror | rule at later chords `<` → `<=` | U |
| 20 | mirror | wrong axis (x → −x) | A |
| 21 | mirror | table built for the first span only, stale afterwards | U |
| 22 | mirror | threshold recorded at the wrong depth | U |
| 23 | ordering | next candidate starts at ci+2 instead of ci+1 | U |
| 24 | ordering | candidate loop stops one early | U |
| 25 | ordering | canonical search starts at candidate 1 instead of 0 | U |
| 26 | chords | chords generated from i+3 (most span-2 chords missing) | U |
| 27 | chords | cycle edge (0,N−1) offered as a chord | S for verdicts; emits non-chord sets |
| 28 | filter | path length L−1 → L−2 | U |
| 29 | filter | path length L−1 → L | U |
| 30 | filter | walks instead of simple paths | S (filter only weakens) |
| 31 | filter | adjacency omits the first chord | U |
| 33 | filter | applied one level too early | U |
| 34 | filter | length 3 never used as a target | S |
| 36 | filter | second target is L1+1 whether or not it is missing | U |
| 37 | slicing | modulus m → m+1 (visible only in sliced runs) | U |
| 38 | driver | no special case for K = 0 (historical bug) | U |
| 39 | mirror | threshold never recorded | S (the first-chord rule alone is a valid canonical form) |
| 41 | driver | length state not rebuilt between spans | U |
| 46 | size | 32-bit shift for the chord's edge bit (wrong only for n + j ≥ 32) | U at large n only |
| 47 | size | filter's path table truncated to 32 bits (wrong only for n > 32) | U at large n only |

## Gates

| gate | what it does | what it can see |
|---|---|---|
| A | the program's own assertion, or a crash | internal inconsistency |
| G1 | recompute A105206(n) for n = 3..22 and compare with the 20 published terms | a wrong *value* in the published range |
| G2 | enumerate every solution on small (n, k), close under the 2n symmetries, compare with the naive reference set; also a sliced run | any lost or bogus solution at small size |
| G3a / G3b | cycle spectrum of random chord sets against the reference, n ≤ 16 / n ≤ 46 | spectrum code only |
| G4 | every witness printed during G1 is re-checked by the reference | wrong acceptance |
| G5a / G5b | planted solutions (goal = spectrum of a random chord set) must be retained, n ≤ 16 / n = 33..41 | lost solutions, at any size |

Not a gate but recorded for every mutant that passes G1: its verdict at
(n, k) = (23, 4) and (24, 4), the first two values past the published range, where
an independently verified witness exists.

## What this design cannot show

The mutants are hand-enumerated single-site edits of one program by the person who
wrote it. They are not a sample of the bugs agents actually make, and kill rates
here say nothing about multi-site or design-level errors.
