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
| A364132 | 22 terms, to $a(22)=52$ | $a(23)=56$, $a(24)=60$, $a(25)=63$, $a(26)=67$ |

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

**New terms.** A105206 was known for $n=3\ldots22$. It is now determined for
$n=23\ldots114$:

| $n$ | 3 | 4–5 | 6–8 | 9–14 | 15–24 | 25–40 | 41–67 | 68–114 | $\ge115$ |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| $h(n)$ | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | $\ge8$ |

$n\le40$ came from searching chord sets on $C_n$ for one $n$ at a time. That
stalled at $n=41$ (22.8 core-minutes for a single value).

**The second reformulation** (`erdos1016/arcform/`) removes $n$ from the search.
Describe a chord set by the cyclic pattern of its chord endpoints (its *type*;
there are 1, 3, 14, 103, 1236, 21878, 496875 types for 1 to 7 chords) and by the
lengths of the arcs between endpoints. Every cycle length is then a fixed linear
form in the arc lengths. Short cycles use few short arcs and long cycles skip
few short arcs, so the lengths can be covered from both ends, and one run decides
every $n$ at once: "five chords never suffice for $n\ge41$" takes 17,378 search
nodes.

**The by-product is exact data on Erdős's own question.** Let $N(k)$ be the
largest $n$ for which a pancyclic graph with $n+k$ edges exists:

| $k$ | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| $N(k)$ | 3 | 5 | 8 | 14 | 24 | 40 | 67 | 114 |
| $N(k)/2^k$ | 3 | 2.5 | 2 | 1.75 | 1.5 | 1.25 | 1.05 | 0.89 |

The ratio falls at every step, the direction Erdős believed ($h(n)-\log_2 n\to\infty$)
but could not prove. Counting cycles would allow it to stay near 2. This is
evidence from eight values, not a proof; details and caveats in
`erdos1016/results.md`. $N(7)$ comes from one program and has not been
reproduced independently.

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
  being trusted on a new one;
* the type search was compared with a naive enumeration on complete solution sets
  (2.4 million chord sets), with the per-$n$ searcher, with a second solver and
  with CP-SAT. Reproducing the known values alone had let two bugs through; see
  `erdos1016/arcform/README.md`.

## Layout

```
survey/      how the problems were selected (Erdős DB + OEIS mining)
erdos357/    Golomb-ruler searcher, verifier, results
erdos1016/   pancyclic searcher, independent verifier, results
erdos1016/arcform/   the search that does not fix n (types, arc lengths, both-ends covering)
research/agentic-math/   notes on agentic mathematics as a research field; the mutation study of verification gates
```
