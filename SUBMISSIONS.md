# Ready-to-file additions to the OEIS

Everything below is backed by `*/certificates.json` and re-derivable with
`*/verify_all.py`. Terms marked new were not present in the entries as of
2026-09-04 (checked against the live entries and their b-files).

---

## A105206 — minimum edges of a pancyclic graph on n vertices

**New terms** (offset 3, continuing `3, 5, 6, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26`):

```
27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40
```

i.e. a(23)=27, a(24)=28, a(25)=30, a(26)=31, a(27)=32, a(28)=33, a(29)=34,
a(30)=35, a(31)=36, a(32)=37, a(33)=38, a(34)=39, a(35)=40.

**Suggested comment.** A pancyclic graph contains an n-cycle, hence is
Hamiltonian, hence is C_n plus a(n)-n chords; and its cycle space has dimension
a(n)-n+1, so it has at most 2^(a(n)-n+1)-1 cycles altogether and its complete
cycle spectrum is the set of GF(2)-combinations of the fundamental cycles. Both
facts together reduce the determination of a(n) to a finite search over chord
sets, which is how the terms above were obtained. (The second fact is also
exactly Bondy's lower bound a(n)-n >= log_2(n-1)-1.)

**Example witnesses** (chords added to the cycle 0-1-...-(n-1)-0):

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

*Note for the editors:* the entry's name says "on n+2 vertices", but the data and
the entry's own example are indexed by the number of vertices itself (a(3)=3 is
the triangle). The name looks like a leftover and may be worth correcting.

---

## A364132 — monotone case of Erdős problem 357

**New terms** (continuing `..., 41, 47, 50, 52`): `56, 60, 63`
— a(23)=56, a(24)=60, a(25)=63.

## A364153 — arbitrary-order case of Erdős problem 357

**New terms** (continuing `..., 14, 17, 18`): `20, 22`
— a(14)=20, a(15)=22.

**Suggested comment for both.** Writing P_0=0 and P_i=a_1+...+a_i, a sum
a_u+...+a_v equals P_v-P_{u-1}, so all such sums are distinct precisely when
{P_0,...,P_k} is a Sidon set, i.e. a Golomb ruler with k+1 marks; the constraint
a_i <= n says every gap of that ruler is at most n. Hence A364153(k) is the
minimum, over all Golomb rulers with k+1 marks, of the largest gap, and
A364132(k) is the same minimum restricted to rulers whose gaps increase.

**Example witnesses:**

* A364153(14)=20: 1, 4, 6, 17, 15, 14, 20, 16, 19, 18, 8, 13, 9, 3
* A364153(15)=22: 1, 11, 16, 8, 21, 22, 20, 19, 15, 18, 14, 17, 6, 3, 4
* A364132(23)=56: 1, 4, 6, 8, 24, 27, 33, 35, 36, 37, 39, 40, 41, 44, 45, 46, 48, 49, 52, 53, 54, 55, 56
* A364132(24)=60: 1, 6, 12, 15, 30, 31, 36, 37, 40, 41, 42, 43, 44, 46, 47, 49, 50, 53, 54, 55, 56, 58, 59, 60
* A364132(25)=63: 2, 4, 8, 10, 13, 33, 34, 41, 42, 43, 44, 45, 47, 48, 49, 51, 52, 53, 54, 55, 57, 59, 61, 62, 63
