# Pancyclic graphs with few edges, searched without fixing n

Erdős problem 1016 / OEIS A105206. Let h(n) be the least h such that some graph on
n vertices with n + h edges is pancyclic (has a cycle of every length 3..n).

The earlier programs in this repository fix n and place chords on the cycle C_n.
That costs more with every n and says nothing about larger n. The programs here
fix only the number of chords k and decide **all n at once**.

## Results

| k | largest n with a pancyclic graph on n vertices and n + k edges | such graphs at that n, up to symmetry |
|---:|---:|---:|
| 0 | 3 | 1 |
| 1 | 5 | 1 |
| 2 | 8 | 3 |
| 3 | 14 | 6 |
| 4 | 24 | 2 |
| 5 | 40 | 5 |
| 6 | **67** | 3 |
| 7 | **114** | 2 |

Rows 0 to 5 confirm what the per-n searches had found. Rows 6 and 7 are new.
With one verified witness for every n in between, this gives

* h(n) = 6 for 41 ≤ n ≤ 67, so A105206(n) = n + 6 there,
* h(n) = 7 for 68 ≤ n ≤ 114, so A105206(n) = n + 7 there,
* h(n) ≥ 8 for every n ≥ 115.

The OEIS entry has 20 terms (n = 3..22). The earlier work here reached n = 40.
This reaches n = 114.

How far each statement is checked is in "What is established" below. In short:
the row k = 5 is confirmed four ways, the row k = 6 by this program and partly by
a second one, the row k = 7 by this program only.

## The idea

A pancyclic graph is Hamiltonian, so it is a cycle C_n plus k chords.

**Type.** Take the p distinct endpoints of the chords in cyclic order and record
which pairs are joined. That is the *type*: p positions and k pairs, every
position used, up to rotation and reflection. The number of types depends only
on k:

| k | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| types | 1 | 3 | 14 | 103 | 1,236 | 21,878 | 496,875 | 13,550,514 |
| most cycles besides the outer one | 2 | 6 | 14 | 28 | 55 | 108 | 212 | 400 |

**Arc lengths.** What remains of the graph is the lengths a_0, ..., a_{p-1} ≥ 1 of
the arcs between consecutive endpoints, with n = a_0 + ... + a_{p-1}. An arc whose
two ends are joined by a chord has length at least 2.

**Cycles are linear forms.** Every cycle of the graph is a cycle of the small
multigraph on the p positions (arcs and chords as edges). A cycle that uses m
chords and the arcs in a set A has

    length = m + sum of a_i over A        defect = n - length = (sum of the arcs it skips) - m.

So a type has at most 2^(k+1) - 2 such forms (the outer cycle excluded), and the
graph is pancyclic exactly when the lengths take every value 3..n-1, that is,
when the defects take every value 1..n-3. Nothing here mentions n.

**Search from both ends** (`arcdfs2.c`). Short cycles use few short arcs. Long
cycles skip few short arcs. Assign arcs step by step. Let L be the smallest
length no fully assigned cycle has yet, D the smallest defect likewise. Then
either some cycle has length L, which fixes the unassigned arcs of that cycle to
a small sum, or none has, and then n ≤ L. The same holds for D with the arcs a
cycle skips, and n ≤ D + 2. At each step the program uses whichever of the two
case distinctions has fewer cases. A branch is cut when n ≥ NMIN is impossible
by counting: n - 3 different lengths are needed, and cycles of unknown length
that share the same unassigned arcs and the same partial sum will end up equal.

Each solution is produced exactly once and is re-verified from scratch before it
is counted.

| question | cycle-by-cycle from the bottom only (`arcdfs.c`) | from both ends (`arcdfs2.c`) |
|---|---:|---:|
| 5 chords, any n ≥ 41? | 3.5 million nodes, 6.6 s | 17,378 nodes, 0.03 s |
| 5 chords, all solutions with n ≥ 30 | 36.4 million nodes, 41 s | 2.2 million nodes, 4.3 s |
| 6 chords, any n ≥ 68? | about 12 core-hours (still running) | 0.94 million nodes, 2.7 s |
| 6 chords, all solutions with n ≥ 57 | not run | 31.4 million nodes, 125 s |
| 7 chords, all solutions with n ≥ 107 | not feasible | 409 million nodes, about 1.2 core-hours |

For comparison, the per-n searcher needed 22.8 core-minutes to rule out 5 chords
at the single value n = 41.

## Numbers of solutions near the top

An entry counts arc-length vectors, so a graph whose type has a symmetry can be
counted twice.

| k | n : solutions |
|---|---|
| 5 | 36: 1199, 37: 400, 38: 159, 39: 15, 40: 7, none above |
| 6 | 62: 1374, 63: 685, 64: 74, 65: 27, 66: 6, 67: 3, none above |
| 7 | 107: 4025, 108: 2488, 109: 784, 110: 168, 111: 106, 112: 16, 113: 5, 114: 2, none above |

The extremal graphs for k = 6 and 7 (chords added to the cycle 0, 1, ..., n-1):

    n = 67    (0,2) (0,60) (1,13) (3,61) (4,31) (59,62)
    n = 67    (0,4) (0,5) (1,61) (2,62) (3,15) (6,33)
    n = 67    (0,7) (1,8) (2,10) (2,11) (9,21) (12,39)
    n = 114   (0,2) (0,111) (1,42) (3,98) (7,110) (33,99) (41,113)
    n = 114   (0,2) (1,42) (1,44) (3,57) (7,45) (33,56) (41,113)

## What the numbers say about Erdős's question

Erdős asked for the growth of h(n) and could not prove that h(n) - log2 n tends
to infinity. Write N(k) for the largest n with h(n) ≤ k (the first column of the
table at the top). Counting cycles alone gives N(k) ≤ 2^(k+1) + 1.

| k | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| N(k) | 3 | 5 | 8 | 14 | 24 | 40 | 67 | 114 |
| N(k) / 2^k | 3 | 2.5 | 2 | 1.75 | 1.5 | 1.25 | 1.05 | 0.89 |
| N(k) / N(k-1) | | 1.67 | 1.60 | 1.75 | 1.71 | 1.67 | 1.68 | 1.70 |

N(k) / 2^k falls at every step, which is the direction "h(n) - log2 n grows".
The formula 2^(k-2) (10 - k) fitted k = 2..5 exactly and predicted 64 and 96; the
true values 67 and 114 are larger, so the fall is slowing. Each step multiplies
N by about 1.7, not 2. The known upper bound h(n) ≤ log2 n + log* n + O(1) means
this ratio cannot stay below 2 forever. These are eight data points and no proof
of anything about the limit.

## What is established

**Upper halves (witnesses).** One witness for every n = 41..114 is in
`witnesses_k6_n41_67.json` and `witnesses_k7_n68_114.json`. Each was checked with
networkx by enumerating all simple cycles, with no use of the type argument.
These halves are certain.

**Lower halves ("no graph with k chords beyond N(k)").** These rest on the search
being complete. The checks:

1. *Types.* `gentypes.c` and `arcform.py` were written separately and print the
   same types and forms for k ≤ 6. The number of labelled patterns equals the
   inclusion-exclusion count, and the orbit sizes of the printed types add up to
   it (k ≤ 8).
2. *Forms.* For 917 sampled types (k = 3, 5, 6, 7; all numbers of endpoints) and
   2,751 random arc vectors, the lengths given by the forms equal the lengths of
   all simple cycles found by networkx (`check_forms.py`).
3. *Complete solution sets against a naive reference.* `arcdfs.c`, closed under
   the symmetries of the cycle, equals the reference enumeration on all 49 cells
   of the gate grid (k ≤ 5, 2,427,201 chord sets). `arcdfs2.c` against the
   reference for k = 6 (n ≤ 12) and k = 7 (n ≤ 11): see the log at the end.
4. *Against the per-n searcher*, which shares no idea with these programs:
   identical solution sets for k = 4 at n = 17..24 and for k = 5 at n = 38, 39, 40
   (0.03 s here against 528 s and 638 s there for n = 38 and 39). The per-n
   searcher also ruled out 5 chords at n = 41, 42, 43, 44 one by one.
5. *`arcdfs2` against `arcdfs`*, line for line: all solutions for k ≤ 4, for k = 5
   with n ≤ 13 (430,387) and n ≥ 30 (76,143), and on a sample of k = 6 types (751).
6. *CP-SAT* (`solve.py`): the same forms as an integer program. For k ≤ 4 it
   returns the maxima 5, 8, 14, 24; for k = 5 and n ≥ 41 every one of the 329
   types with enough cycles is infeasible.
7. *Bookkeeping.* `runk.sh` compares the number of types searched with the
   number the generator reports.

What each row of the table therefore rests on:

| claim | checked by |
|---|---|
| no 5 chords for n ≥ 41 | `arcdfs2`, `arcdfs`, CP-SAT, and the per-n searcher for n = 41..44 |
| no 6 chords for n ≥ 68 | `arcdfs2`; `arcdfs` on the parts finished so far (see `xcheck/`) |
| no 7 chords for n ≥ 115 | `arcdfs2` only |

The k = 7 row has not been reproduced by a second program. Treat N(7) = 114 and
"h(n) ≥ 8 for n ≥ 115" as computed, not as independently confirmed. The values
h(n) = 7 for 68 ≤ n ≤ 114 do not depend on it: they need the k = 6 row and the
witnesses.

None of this is a formal proof. A proof log that a separate checker can replay
would be the next step; the k = 5 refutation has only 17,378 cases.

## Three bugs that the usual check did not see

Recorded because they bear on the question studied in
`research/agentic-math/p2-gates`.

The first working version of `arcdfs.c` returned the known maxima 5, 8, 14, 24
for k = 1..4 and was wrong twice: it cleared arc values that outer loops still
owned, and it ignored cycles made of chords only. Comparing the *complete*
solution sets with the reference showed both at once (11 of 46 cells differed,
all with k = 4). Reproducing the known answers had not.

The run script accepted the output of two killed processes as finished parts,
because the exit status of a pipeline is that of its last command. The k = 7 run
looked complete with 201,095 types searched; the generator had 208,031. The
count comparison is now part of the script.

## Reproduce

```
gcc -O2 -o gentypes gentypes.c && gcc -O2 -o arcdfs2 arcdfs2.c && gcc -O2 -o arcdfs arcdfs.c
./gentypes 5 | ./arcdfs2 41                 # no 5-chord graph with n >= 41       (0.03 s)
./gentypes 6 | ./arcdfs2 57 --print         # all 6-chord graphs with n >= 57     (2 min)
./runk.sh 7 107 60 4 k7_min107              # all 7-chord graphs with n >= 107    (about 1.2 core-hours)
./gentypes 7 --part 3 1500 | python3 check_forms.py 3 7        # forms against networkx
./cmp_ref.sh 6 12                           # against the naive reference (needs ../../research/agentic-math/p2-gates/refs)
python3 solve.py 5 --min 41                 # the same question for CP-SAT        (25 min)
```

Files: `arcform.py` (types and forms, slow and plain), `gentypes.c` (the same,
fast), `arcdfs.c` (first solver), `arcdfs2.c` (two-ended solver), `solve.py`
(CP-SAT), `check_forms.py`, `cmp_ref.sh`, `verify_witnesses.py`, `orbits.py`,
`runk.sh`, `runchunks.sh`, `k7_min107/` (all 7,594 solutions with 7 chords and
n ≥ 107), `xcheck/` (cross-check logs).
