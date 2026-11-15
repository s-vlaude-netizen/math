# Graphs that are the union of two threshold graphs

Counts of labeled and unlabeled graphs on n <= 8 nodes with threshold dimension at most 2. Paste-ready drafts: `DRAFTS.md`.

| n | labeled | unlabeled | all graphs (A000088) |
|---:|---:|---:|---:|
| 1 | 1 | 1 | 1 |
| 2 | 2 | 2 | 2 |
| 3 | 8 | 4 | 4 |
| 4 | 64 | 11 | 11 |
| 5 | 1,012 | 33 | 34 |
| 6 | 28,084 | 133 | 156 |
| 7 | 1,169,611 | 660 | 1,044 |
| 8 | 65,535,747 | 4,090 | 12,346 |

## Method

* `gen.py` builds all labeled threshold graphs on n nodes (add the last node as isolated or dominating, deduplicate). The
  counts equal A005840 for n = 1..8 (1, 2, 8, 46, 332, 2874, 29024, 334982).
* `unions.c` ORs all pairs, marks the results in a table of 2^(n(n-1)/2) bits (32 MB for n = 8, about 5.6 * 10^10 pairs),
  counts the marked graphs, and counts isomorphism classes with Burnside's lemma over the cycle types of S_n.

## Checks

* The Burnside code reproduces 2^(n-1) unlabeled threshold graphs for n <= 8 and A000088 (1, 2, 4, 11, 34, 156, 1044)
  for all graphs, n <= 7.
* `check.py` (n <= 6) recomputes the threshold graphs by a different method (no induced 2K2, C4, P4), checks that the list
  equals the generated one, recounts the unions, and counts isomorphism classes by brute-force canonical forms over all
  n! relabelings. All agree: 8, 64, 1012, 28084 labeled; 4, 11, 33, 133 classes.
* The programs in `prog_*.py` print the terms for n <= 5.
* At n = 5 the 12 graphs outside the class are exactly the 12 labeled 5-cycles.

## Limits

The terms for n = 7 and 8 come from the C program only (tested as described, not recomputed by a second method).
