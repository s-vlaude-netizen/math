# Threshold networks with few hidden neurons

Counts of Boolean functions that a one-hidden-layer network of linear threshold neurons can compute.
Paste-ready OEIS drafts: `DRAFTS.md`.

## Definitions

A *threshold function* of n variables is a Boolean function `f(x) = [w.x >= t]`. Their number is A000609:
2, 4, 14, 104, 1882, 94572 (n = 0..5). A *network with k hidden neurons* computes `g(h_1, ..., h_k)` where every h_i
is a threshold function of the inputs and g is a threshold function of the k hidden outputs.

Because threshold functions are closed under negation, two hidden neurons compute exactly the functions that are the
AND of two threshold functions, together with their complements.

## Results

| n | threshold functions (k = 1) | AND of two threshold functions | two hidden neurons (k <= 2) | not computable with 2 |
|---:|---:|---:|---:|---:|
| 1 | 4 | 4 | 4 | 0 |
| 2 | 14 | 16 | 16 | 0 |
| 3 | 104 | 246 | 254 | 2 |
| 4 | 1,882 | 32,506 | 41,614 | 23,922 |
| 5 | 94,572 | 44,642,388 | 74,986,552 | 4,219,980,744 |

Number of functions that need exactly k hidden neurons (rows n = 1..4):

| n | k = 1 | k = 2 | k = 3 | k = 4 |
|---:|---:|---:|---:|---:|
| 1 | 4 | | | |
| 2 | 14 | 2 | | |
| 3 | 104 | 150 | 2 | |
| 4 | 1,882 | 39,732 | 23,920 | 2 |

The functions at the far end are the parity functions: the n-bit parity needs n hidden neurons for n <= 4. All other
functions of 4 variables need at most 3. For n = 5 only the first two entries are known (94,572 and 74,891,980).

## How it was computed

* `thr.py` lists all threshold functions of n variables as truth tables, from integer weights with |w| <= B. The count
  is compared with A000609; reaching it proves B was large enough (B = 3 for n <= 4, B = 5 for n = 5; for n = 4 with
  B = 2 only 986 of 1,882 are found, so the test can fail).
* `andpairs.c` forms all ANDs of two threshold functions and marks them in a table of 2^(2^n) bits (512 MB for n = 5);
  about 4.5 * 10^9 pairs for n = 5, under two minutes.
* `hidden.py` decides, for n <= 4, the least k for every function. Functions are grouped into classes under permuting
  and negating inputs and negating the output (4, 14 and 222 classes for n = 2, 3, 4; the minimum k is the same inside
  a class). k = 1 and k = 2 come from the tables above. For k >= 3 the question "is there a network with k hidden
  neurons" is given to CP-SAT as an integer program with all weights bounded as in `thr.py`.

## What was checked

* Threshold counts equal A000609 for n = 1..5.
* For n <= 4 the number of functions needing exactly 1 and 2 hidden neurons from the integer program's classification
  equals the counts from the tables (104 / 150, 1,882 / 39,732) and the class sizes add up to 2^(2^n).
* Every network the solver reported (k = 3 and k = 4 cases) was re-evaluated point by point against the function.
* `andverify.c` decides "is f the AND of two threshold functions" for random f by a different method (collect the
  threshold functions containing f, look for a pair whose AND is f) and compares with the table. No mismatch for
  n = 3 (60,000 samples), n = 4 (60,000) and n = 5 (about 182,000 decided; uniform random functions are in the set
  with frequency 0.993 %, the table says 1.039 %, which is within the sampling error of about 0.03 %).
* The n = 5 run was done twice and gave identical numbers.
* The programs in `prog_*.py` reproduce the terms they print (n <= 4, rows <= 3).

## Limits

* The n = 5 values come from one program and the sample check, not from a second full computation.
* "k = 3 is impossible for the 4-bit parity" and the like rest on CP-SAT's infeasibility proofs. The weight bounds are
  justified by the completeness test above; the solver is trusted.
* Row 5 of the triangle beyond its first two entries is not known.

## A false start

I first counted the "convex" subsets of the cube's vertices, the sets S with S = conv(S) intersected with the vertices.
Every vertex of a cube is an extreme point, so all 2^(2^n) subsets are convex; the count (256 for n = 3, 65,536 for
n = 4) is meaningless and was discarded. The right question is how many half-spaces are needed, which is what the
tables above count.
