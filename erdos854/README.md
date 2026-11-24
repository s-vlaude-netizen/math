# Erdős problem 854 — which even numbers are gaps between consecutive reduced residues of a primorial?

[Problem 854](https://www.erdosproblems.com/854) · OEIS [A389839](https://oeis.org/A389839)

Let `P_n = prime(1)·…·prime(n)` and let `a_1 < a_2 < …` be the integers coprime to `P_n`.
Erdős asked for the **smallest even number that is not a gap `a_{i+1} − a_i`**, and whether
every even number up to the largest gap (the Jacobsthal function, A048670) occurs. He
reported that Lacampagne and Selfridge found it fails for `P_6 = 30030`.

**Prior work (found after the computation below was done).** Mario Ziller,
[On differences between consecutive numbers coprime to primorials](https://arxiv.org/abs/2007.01808)
(2020), solves the same restricted-covering problem and tabulates, for `k ≤ 44`, every
even number below the Jacobsthal value that is not a gap (his Table 1). So the *values*
below are not new mathematics. What this directory adds: the OEIS entry A389839 stops at
n = 12, and the terms n = 13..44 can be filled in from that table; an independent
implementation reproduces Ziller's table for n = 13..23 (all of it: the smallest missing
gap, and where I listed them, the whole set of missing gaps), which is a check on the
table as well as on the code.

## Reformulation (exact, and independent of the size of `P_n`)

An even `g` is a gap iff there is an integer `x` with `x` and `x+g` coprime to `P_n` and every
`x+i`, `0 < i < g`, sharing a factor with `P_n`. By the Chinese remainder theorem `x` is
described by one residue per prime, so:

> `g` is a gap iff for every prime `p ≤ prime(n)` one can choose a residue `s_p` with
> `s_p ≢ 0` and `s_p ≢ g (mod p)` (so `x` and `x+g` are not hit) such that the classes
> `i ≡ s_p (mod p)` together cover every `i = 1, …, g−1`.

This is a covering problem of size `g`, not of size `P_n ≈ 10^30`. Deciding it is an
exhaustive search over which prime covers which position. Two independent solvers:

* `gaps3w.c` — branches over the primes in increasing order; a branch is cut when the
  best single class of each remaining prime cannot cover what is still uncovered. Fast.
* `gaps2.cpp` — branches over *positions* (the uncovered position with fewest candidate
  primes), cut by the same kind of counting bound. Slower, different search tree.

## Results

(`n` as in the OEIS entry, whose offset is 2: `a(n)` concerns the primes up to `prime(n)`.)

| n | prime(n) | a(n) | largest gap A048670(n) | smallest missing < largest gap? |
|--:|--:|--:|--:|:--:|
| 2..12 | 3..37 | 6, 8, 12, 16, 20, 28, 32, 42, 48, 60, 68 (published) | | n = 6, 8 |
| 13 | 41 | **76** | 74 | no |
| 14 | 43 | **86** | 90 | **yes** |
| 15 | 47 | **98** | 100 | **yes** |
| 16 | 53 | **108** | 106 | no |
| 17 | 59 | **120** | 118 | no |
| 18 | 61 | **134** | 132 | no |
| 19 | 67 | **146** | 152 | **yes** |
| 20 | 71 | **166** | 174 | **yes** |
| 21 | 73 | **182** | 190 | **yes** |
| 22 | 79 | **194** | 200 | **yes** |
| 23 | 83 | **218** | 216 | no |

When `a(n) = A048670(n) + 2` every even number up to the largest gap occurs (Erdős's first
guess); otherwise it fails. It fails for n = 6, 8, 14, 15, 19, 20, 21, 22 (and, from Ziller's table, 24, 25, 27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 40, 42, 43).

## How the results were checked

* `brute.c` computes the gaps of the coprime sequence directly (no covering model) and
  reproduces `a(n)` for n = 2..9.
* Both solvers reproduce all 11 published terms (n = 2..12).
* For n = 2..17 `gaps3x` writes a witness `(s_p)` for **every** attained even gap below `a(n)`
  (`witnesses_2_17.txt`); `verify_witnesses.py` rebuilds `x` by CRT and checks with real
  gcds on the integers `x, …, x+g` (398 witnesses, 0 failures).
* The solver finds the largest gap `h = A048670(n)` attained and `h+2` not attained for
  n = 2..22, i.e. it re-derives the Jacobsthal values of the primorials in both directions.
* `gaps2` (different branching) agrees with `gaps3w` for n = 13..16; larger n rest on
  `gaps3w` alone plus the checks above.

Agreement with Ziller's Table 1 (arXiv:2007.01808): for every n from 13 to 23 the smallest
non-existent difference listed there (or `h(n)+2` where the table lists none) equals the `a(n)`
above, and the missing sets I enumerated for n = 14 ({86, 88}) and n = 15 ({98}) are his.

Run: `gcc -O2 -march=native -o gaps3w gaps3w.c && ./gaps3w 2 22`.
