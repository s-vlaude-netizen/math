# Two open Erdős problems, pushed a little further

This repository is the record of a search for open mathematical problems whose
*computational frontier* is close enough to move, and of the attack on the two
that were selected.

Nothing here resolves a conjecture. What it does is extend the exact data on two
open problems of Erdős beyond everything published, using reformulations that
make exhaustive search possible where brute force is hopeless.

## How the problems were chosen

`survey/` mines the community [Erdős problem database](https://github.com/teorth/erdosproblems)
(1217 problems, 604 open) and cross-references it with the OEIS: 214 open
problems carry an associated sequence, and the sequences tagged `more` — the
editors want further terms — with only a handful of known terms mark exactly the
places where nobody has computed the next value.

Two problems were picked from that shortlist, in both cases because a
reformulation collapses the search space.

## Erdős 357 — sequences with distinct consecutive sums

[Problem 357](https://www.erdosproblems.com/357) · `erdos357/` ·
[A364132](https://oeis.org/A364132), [A364153](https://oeis.org/A364153)

> Let $1\le a_1<\cdots<a_k\le n$ with all sums $\sum_{u\le i\le v}a_i$ distinct,
> and let $f(n)$ be the maximal such $k$. How does $f(n)$ grow? Is $f(n)=o(n)$?

**The reformulation.** Passing to the partial sums $P_i=a_1+\cdots+a_i$ turns a
consecutive sum into a difference $P_v-P_{u-1}$, so *all consecutive sums are
distinct exactly when the partial sums form a Sidon set* — a Golomb ruler. The
condition $a_i\le n$ bounds its gaps. Both sequences therefore ask:

> minimise the largest gap of a Golomb ruler with $k+1$ marks.

**New terms** (all previously published terms reproduced first):

| | published | new |
|---|---|---|
| A364153 | 13 terms, to $a(13)=18$ | $a(14)=20$, $a(15)=22$, $a(16)=24$ |
| A364132 | 22 terms, to $a(22)=52$ | $a(23)=56$, $a(24)=60$, $a(25)=63$ |

## Erdős 1016 — how sparse can a pancyclic graph be?

[Problem 1016](https://www.erdosproblems.com/1016) · `erdos1016/` ·
[A105206](https://oeis.org/A105206)

> $h(n)$ = least $h$ with a graph on $n$ vertices, $n+h$ edges, containing a cycle
> of every length $3\le k\le n$. Estimate $h(n)$.

Erdős could not even prove $h(n)-\log_2 n\to\infty$.

**The reformulation.** A pancyclic graph contains an $n$-cycle, so it is
Hamiltonian: every candidate is $C_n$ plus $k$ chords. And its cycle space has
dimension only $k+1$, so it has at most $2^{k+1}-1$ cycles *in total* and its
whole cycle spectrum is read off from that many bitmasks — no cycle enumeration.
A counting prune and a rotation symmetry reduction do the rest.

**New terms:** A105206 was known for $n=3\ldots22$; $h(n)$ is now determined for
$n=23\ldots40$ as well:

```
n     23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
a(n)  27 28 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45
```

Up to $n=33$ the lower bound is an exhaustive refutation of every chord set one
size smaller; from $n=34$ on it follows from Bondy's counting bound, and only a
witness is needed.

The by-product is evidence on Erdős's own question. Writing $N(k)$ for the
largest $n$ with $h(n)=k$, the computed values give $N(k)/2^k = 3,\,2.5,\,2,\,1.75,\,1.5$
for $k=0..5$ (the last being $\le 1.25$) — falling at every step, where Bondy's counting bound alone would
permit it to stay near 2 forever. That is the direction Erdős believed but could
not prove.

## Trusting the numbers

Every claim here is machine-checked twice, by deliberately different means:

* the Golomb-ruler witnesses are re-expanded into all $k(k+1)/2$ consecutive sums
  and tested for distinctness (`erdos357/verify.py`);
* the pancyclic witnesses are re-checked by enumerating *all* simple cycles with
  networkx, i.e. without using the cycle-space argument at all, and the two
  spectrum computations were compared on 4000 random graphs with no mismatch
  (`erdos1016/verify_pancyclic.py`);
* the $n=25$ refutation was run both with and without the symmetry reduction
  (233 829 950 vs 10 997 097 nodes, same verdict);
* both searchers reproduce **every** published term of their sequences before
  being trusted on a new one.

## Layout

```
survey/      how the problems were selected (Erdős DB + OEIS mining)
erdos357/    Golomb-ruler searcher, verifier, results
erdos1016/   pancyclic searcher, independent verifier, results
```
