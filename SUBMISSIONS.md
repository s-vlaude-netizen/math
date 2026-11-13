# Ready-to-file additions to the OEIS

Everything below is backed by `*/certificates.json` and re-derivable with
`*/verify_all.py`. None of these terms were present in the entries as of
2026-11-13 (A105206 still has the 20 terms n = 3..22).

**Nothing has been submitted.** Contributing needs an OEIS account. As of
2026-11-13 registration is the "Register" link at the top of https://oeis.org
(it leads to https://oeis.org/account; sign-in with Google, GitHub or an e-mail
address). The OEIS asks contributors to use their real name. After logging in,
open the sequence page and use "edit"; changes are reviewed by the editors
before they appear.

---

## A105206 — minimum number of edges of a pancyclic graph on n vertices

**New terms** (offset 3, continuing `3, 5, 6, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26`):

```
27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121
```

That is a(n) = n+4 for 15 <= n <= 24, n+5 for 25 <= n <= 40, n+6 for 41 <= n <= 67,
n+7 for 68 <= n <= 114. A b-file for n = 3..114 is `erdos1016/b105206.txt`.

How each range was obtained:

* n = 23..33: a witness, and an exhaustive refutation of every chord set one
  size smaller on C_n, up to rotation.
* n = 34..40: a witness; the lower bound follows from the counting bound below.
* n = 41..67: a witness for each n; no pancyclic graph "cycle plus 5 chords"
  exists for any n >= 41 (search over the 1236 chord patterns, see suggested
  comment 2; confirmed by two further solvers and, for n = 41..44, by the
  per-n search).
* n = 68..114: a witness for each n; no pancyclic graph "cycle plus 6 chords"
  exists for any n >= 68 (search over the 21878 chord patterns; a second solver
  had confirmed part of this run when this file was written, see
  `erdos1016/arcform/xcheck/`).

A cautious first submission would stop at n = 67, where the lower bound has been
confirmed four ways, and add n = 68..114 once the second solver has finished.

**Suggested comment 1.** A pancyclic graph contains an n-cycle, hence is
Hamiltonian, hence is C_n plus a(n)-n chords; and its cycle space has dimension
a(n)-n+1, so it has at most 2^(a(n)-n+1)-1 cycles altogether and its complete
cycle spectrum is the set of GF(2)-combinations of the fundamental cycles. Both
facts together reduce the determination of a(n) to a finite search over chord
sets. (The second fact is also exactly Bondy's lower bound
a(n)-n >= log_2(n-1)-1.)

**Suggested comment 2.** For a fixed number k of chords the search can be made
independent of n: record the cyclic pattern of the chord endpoints (finitely many
patterns for each k) and the lengths of the arcs between consecutive endpoints;
every cycle length is then the number of chords used plus the sum of the arc
lengths used. The largest n for which a pancyclic graph with n+k edges exists is
3, 5, 8, 14, 24, 40, 67, 114 for k = 0..7, so a(n) = n+k for
N(k-1) < n <= N(k) with these values N(k).

**Witnesses** (chords added to the cycle 0-1-...-(n-1)-0):

| n | chords |
|---|---|
| 23 | (0,2) (0,20) (1,18) (8,20) |
| 24 | (0,2) (0,21) (1,19) (8,23) |
| 25 | (0,2) (0,3) (0,4) (0,20) (1,16) |
| 26 | (0,2) (0,3) (0,4) (1,11) (1,22) |
| 27 | (0,2) (0,3) (0,5) (1,9) (3,18) |
| 28 | (0,2) (0,3) (0,5) (1,17) (2,21) |
| 29 | (0,2) (0,3) (0,5) (1,18) (2,22) |
| 30 | (0,2) (0,3) (0,9) (3,11) (4,16) |
| 31 | (0,2) (0,3) (0,10) (5,9) (6,24) |
| 32 | (0,2) (0,3) (0,28) (1,14) (6,31) |
| 33 | (0,2) (0,3) (0,29) (1,14) (6,32) |
| 34 | (0,2) (0,4) (0,21) (1,7) (3,26) |
| 35 | (0,2) (0,10) (1,4) (3,18) (6,34) |
| 36 | (0,2) (0,11) (1,7) (7,10) (8,19) |
| 37 | (9,26) (14,24) (25,28) (9,22) (23,25) |
| 38 | (1,24) (0,35) (24,26) (25,37) (19,36) |
| 39 | (13,31) (11,14) (12,28) (19,30) (29,31) |
| 40 | (20,29) (28,31) (10,25) (26,28) (27,33) |
| 41..67 | `erdos1016/arcform/witnesses_k6_n41_67.json`; e.g. n = 67: (0,2) (0,60) (1,13) (3,61) (4,31) (59,62) |
| 68..114 | `erdos1016/arcform/witnesses_k7_n68_114.json`; e.g. n = 114: (0,2) (0,111) (1,42) (3,98) (7,110) (33,99) (41,113) |

*Note for the editors:* the entry's name says "on n+2 vertices", but the data and
the entry's own example are indexed by the number of vertices itself (a(3)=3 is
the triangle). The name looks like a leftover and may be worth correcting.

---

## Two sequences that are not in the OEIS (searched 2026-11-13, no match)

**Largest n such that a pancyclic graph with n vertices and n+k edges exists**,
k >= 0:

```
3, 5, 8, 14, 24, 40, 67, 114
```

Equivalently one less than the first n with A105206(n) - n > k. The terms for
k <= 5 are confirmed by independent programs, k = 6 partly, k = 7 by one program
only (`erdos1016/arcform/README.md`). I would submit it only up to the last term
that has been reproduced.

**Number of chord patterns with k chords**: ways to choose p points on a circle
and k distinct chords between them so that every point is the end of a chord, up
to rotation and reflection, all p together, k >= 1:

```
1, 3, 14, 103, 1236, 21878, 496875, 13550514
```

(The labelled counts 1, 6, 62, 900, 16824, 384668, 10398480, 324420840 are the
numbers of labelled graphs with k edges and no isolated vertex; the terms above
count their orbits under the dihedral group acting on the cyclically ordered
vertices.) Computed by `erdos1016/arcform/gentypes.c`; the first six terms agree
with an independent Python enumeration.

---

## A364132 / A364153 — Erdős problem 357

**A364132** (monotone case), continuing `..., 41, 47, 50, 52`: `56, 60, 63, 67`
**A364153** (arbitrary order), continuing `..., 14, 17, 18`: `20, 22, 24`

**Suggested comment for both.** Writing P_0=0 and P_i=a_1+...+a_i, a sum
a_u+...+a_v equals P_v-P_(u-1), so all such sums are distinct precisely when
{P_0,...,P_k} is a Sidon set, i.e. a Golomb ruler with k+1 marks; the
constraint a_i <= n says every gap of that ruler is at most n. Hence A364153(k)
is the minimum, over all Golomb rulers with k+1 marks, of the largest gap, and
A364132(k) is the same minimum restricted to rulers whose gaps increase.

**Witnesses:**

* A364153(14)=20: 1, 4, 6, 17, 15, 14, 20, 16, 19, 18, 8, 13, 9, 3
* A364153(15)=22: 1, 11, 16, 8, 21, 22, 20, 19, 15, 18, 14, 17, 6, 3, 4
* A364153(16)=24: 1, 2, 6, 5, 23, 20, 24, 22, 18, 12, 21, 17, 15, 10, 16, 19
* A364132(23)=56: 1, 4, 6, 8, 24, 27, 33, 35, 36, 37, 39, 40, 41, 44, 45, 46, 48, 49, 52, 53, 54, 55, 56
* A364132(24)=60: 1, 6, 12, 15, 30, 31, 36, 37, 40, 41, 42, 43, 44, 46, 47, 49, 50, 53, 54, 55, 56, 58, 59, 60
* A364132(25)=63: 2, 4, 8, 10, 13, 33, 34, 41, 42, 43, 44, 45, 47, 48, 49, 51, 52, 53, 54, 55, 57, 59, 61, 62, 63
* A364132(26)=67: 1, 2, 4, 5, 14, 22, 35, 39, 42, 44, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 61, 63, 64, 65, 66, 67
