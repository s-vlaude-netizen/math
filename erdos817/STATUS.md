# A399720 (Erdős 817), a(7): not reachable with the current search

`ternary.c` finds the least N such that {1..N} holds an n-set with all 3^n sums
`sum c_i a_i` (c_i in {0,1,2}) distinct. Exhaustive exclusion of one maximum N costs:

| n | N tested (= a(n) - 1) | nodes | time |
|--:|--:|--:|--:|
| 3 | 7 | 6 | 5 ms |
| 4 | 21 | 149 | 5 ms |
| 5 | 59 | 13,598 | 12 ms |
| 6 | 167 | 5,360,654 | 10 s |

The ratio between consecutive rows grows (25, 91, 394). For n = 7 the entry gives
419 <= a(7) <= 504, so every N from 419 up to a(7) - 1 (47 values if a(7) = 466, the value the
sibling sequences A318821/A318863 suggest) would have to be excluded one by one. A first
attempt at N = 419, 420, 421 ran for more than 35 CPU-minutes each without finishing, so a
realistic estimate is 10^10..10^12 nodes per N: weeks of CPU. No term was obtained; do not
cite anything from `run7.log`, which is empty.

Ideas that would be needed (none tried): a pruning bound from the lattice formulation
(valid iff the lattice {c : c . a = 0} has no non-zero vector in [-2,2]^n; Minkowski gives
sum a_i^2 >= 3^(2n-2), too weak to prune), or a meet-in-the-middle split into a 3-set and a
4-set with disjoint difference sets.
