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
  exists for any n >= 68 (search over the 21878 chord patterns, confirmed by a
  second, independently written solver; see `erdos1016/arcform/xcheck/`).

Both ranges (to n = 67 and to n = 114) now rest on lower bounds confirmed by two
solvers. Only the 7-chord row (terms of the new sequence beyond a(6) = 67, and
"h(n) >= 8 for n >= 115") comes from a single program.

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

---

## A389839 — Erdős problem 854 (smallest even number that is not a gap between consecutive coprimes to prime(n)#)

Current entry: offset 2, 11 terms `6, 8, 12, 16, 20, 28, 32, 42, 48, 60, 68`.

**Continuation** (n = 13, 14, ...):

```
76, 86, 98, 108, 120, 134, 146, 166, 182, 194, 218, 230, 254, 266, 278, 296, 314, 328, 346, 368, 386, 406, 424, 446, 468, 488, 512, 530, 552, 572, 592, 618
```

**Where the terms come from — say this in the submission.**

* a(13)..a(24) (76 .. 230): computed here with an exhaustive covering search
  (`erdos854/gaps3w.c`, cross-checked by a second solver with a different search tree for
  n <= 16, by direct sieving for n <= 9, and with every smaller attained gap verified by an
  explicit integer `x` and real gcds, n <= 17). They agree with Table 1 of the paper below.
* a(25)..a(44) (254 .. 618): **not computed here**; read off Table 1 of Mario Ziller,
  "On differences between consecutive numbers coprime to primorials", arXiv:2007.01808
  (2020): a(n) is the smallest non-existent difference listed for k = n, or h(n)+2 when
  the table lists none (h = A048670). The solver's run for n = 25 (expected 254) was still going
  at the time of writing; `erdos854/run3w.out` shows how far it got.

**Suggested comments.**

1. g is a difference between consecutive integers coprime to prime(n)# iff one can choose a
   residue s_p mod p for each prime p <= prime(n), with s_p not congruent to 0 or g (mod p),
   such that the classes i == s_p (mod p) cover 1, ..., g-1. (Chinese remainder theorem;
   Ziller phrases it as a restricted covering.)
2. a(n) <= A048670(n) + 2, with equality iff every even number up to A048670(n) is such a
   difference. This fails for n = 6, 8, 14, 15, 19, 20, 21, 22, 24, 25, 27, 28, 30, ..., and
   holds for n = 2..5, 7, 9..13, 16..18, 23, 26, 29, 39, 41, 44.
3. Reference: M. Ziller, arXiv:2007.01808.

---

## A401861 — Erdős problem 272 (families of subsets whose pairwise intersections are non-empty APs)

Current entry: offset 1, 12 terms `1, 2, 4, 7, 12, 17, 23, 30, 39, 48, 58, 69`, all equal to Szabó's lower bound
`C(n,2) + floor((n-1)/4) + 1`.

**New term: not yet.** a(13) >= 82 (Szabó's bound; a family of 82 sets exists). Whether 83 sets are possible is
being decided by `erdos272/clique3` (`n13_lb83_*`); do not submit a(13) until that run reports "none >= 83".

**Suggested comments (computed, exact, `erdos272/`; each is a statement about a restricted class, not about a(n)):**

1. Among families in which all sets contain a common element, the maximum size is exactly C(n,2) + floor((n-1)/4) + 1
   for 11 <= n <= 16 (all choices of the common element were run; it is attained with the element ceil(n/2) and every
   other element gives strictly fewer, e.g. 79, 79, 80, 80, 81, 81 for x = 1..6 at n = 13). For n = 17 and 18 only the
   element ceil(n/2) was run (it gives 141 and 158, again equal to the formula). So no improvement of Szabo's bound can
   come from a family with a common element, for those n.
2. Among families with no common element, the maximum size is C(n,2) + 1 for 5 <= n <= 11 (11, 16, 22, 29, 37, 46, 56).
   An extremal one for n = 8, 9: all 2- and 3-subsets containing a fixed element x except {x}, plus the set {1..n} \ {x}.
3. For 5 <= n <= 11 the two maxima differ by exactly floor((n-1)/4) (1, 1, 1, 1, 2, 2, 2), and that gap grows with n.
   This is consistent with Szabo's conjecture that extremal families have a common element, but it is evidence
   for small n only and does not prove a(n) = C(n,2) + floor((n-1)/4) + 1 for larger n.

Reproduce: `gcc -O3 -march=native -o clique_star clique_star.c && ./clique_star n 0 1 0 x` (star at x) and
`clique_nonstar.c` (no common element).

---

## A402186 — Erdős problem 160 (least number of colours so that every 4-term AP has at least 3 colours)

Current entry: offset 1, 53 terms (checked 2026-11-27): 1,1,1, then 3 (n=4..12), 4 (13..22), 5 (23..35), 6 (36..53).

**Continuation** (n = 54, 55, ...; 21 terms so far):

```
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
```

b-file for n = 1..74: `erdos160/b402186.txt` (its first 53 lines agree with the live entry).

**What each part rests on (state this in the submission).**

* `a(n) <= 7` for 54 <= n <= 74: a 7-colouring of {1..74} (restriction gives every smaller n), found by local search and
  verified by an independent script (`erdos160/verify.py`). It is
  `1 3 4 3 1 5 3 1 2 7 6 6 4 2 4 3 7 5 6 6 7 2 1 1 6 3 5 4 1 1 5 4 7 5 3 7 7 4 2 6 2 4 5 2 6 2 3 1 6 7 1 7 5 5 1 3 4 7 7 6 2 3 3 1 2 2 4 5 6 2 5 6 4 6 `
* `a(n) >= 7` for n >= 54: there is **no** 6-colouring of {1..54}. This is an exhaustive computer search (`erdos160/col6b.c`,
  4.25e9 nodes, all 43,036 tasks completed on 4 cores; checks in `erdos160/results.md`). It is **one program and has no
  certificate**; it reproduces the known thresholds for 3, 4 and 5 colours (12, 22, 35) and its propagation rules were
  tested against exhaustive completion. A second, independent implementation has not been run.
* Further terms: the longest 7-colourable interval is not known (≥ 74). The same search for 7 colours is out of reach
  (estimated tree size 2e15 to 4e15 nodes).

**Suggested comment.** Let N(k) be the largest n for which {1..n} has a k-colouring in which every 4-term AP has at least 3
colours; then a(n) = k iff N(k-1) < n <= N(k). N(3), ..., N(6) = 12, 22, 35, 53, and N(7) >= 74.

**Program** (verifies the first thresholds; k = 5, 6 need `erdos160/col6b.c`):

```
(Python)
def longest(k):
    col = []; best = [0]
    def ok(p):
        return all(len({col[p], col[p-d], col[p-2*d], col[p-3*d]}) >= 3 for d in range(1, p//3 + 1))
    def rec(p, used):
        best[0] = max(best[0], p)
        for c in range(min(used + 1, k)):
            col.append(c)
            if ok(p): rec(p + 1, max(used, c + 1))
            col.pop()
    rec(0, 0)
    return best[0]
print([longest(k) for k in range(1, 5)])  # [3, 3, 12, 22]
```

### New sequence (companion of A402186)

**NAME** Largest n such that the integers 1..n can be coloured with k colours so that every 4-term arithmetic progression
contains at least three distinct colours.

**DATA** (offset 1) `3, 3, 12, 22, 35, 53`

**COMMENTS** Inverse of A402186: a(k) is the largest n with A402186(n) <= k. a(7) >= 74 (a 7-colouring of 1..74 exists).
a(1) = a(2) = 3 because the progression 1, 2, 3, 4 needs three colours. a(3), a(4), a(5) agree with the thresholds visible in
A402186 (12, 22, 35); a(6) = 53 follows from A402186(53) = 6 and the exhaustive search above showing no 6-colouring of 1..54.

**CROSSREFS** A402186. **KEYWORD** nonn,hard,more. (Check that this sequence is still absent before submitting: searched
2026-11-27 for 3,12,22,35,53 with no match.)

Reproduce: `erdos160/README.md`.
