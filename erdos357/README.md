# Erdős Problem 357 — sequences with distinct consecutive sums

<https://www.erdosproblems.com/357> (status: **open**)

> Let $1\le a_1 < \cdots < a_k \le n$ be integers such that all sums of the shape
> $\sum_{u\le i\le v} a_i$ are distinct. Let $f(n)$ be the maximal such $k$.
> How does $f(n)$ grow? Is $f(n)=o(n)$?
>
> — Erdős and Harzheim

$g(n)$ denotes the same quantity when the monotonicity assumption $a_1<\cdots<a_k$
is dropped. Known bounds (Hegyvári 1986, Coppersmith–Phillips):

$$\left(\tfrac13+o(1)\right)n \;\le\; g(n) \;\le\; \left(\tfrac23-\tfrac1{512}+o(1)\right)n .$$

The two inverse functions are catalogued as
[A364132](https://oeis.org/A364132) (monotone, inverse of $f$) and
[A364153](https://oeis.org/A364153) (arbitrary order, inverse of $g$).

## The reformulation that makes this computable

Write $P_0=0$ and $P_i=a_1+\cdots+a_i$. A consecutive sum is exactly a difference
$P_v-P_{u-1}$, so

> **all consecutive sums are distinct $\iff$ $\{P_0,\dots,P_k\}$ is a Sidon set,
> i.e. a Golomb ruler with $k+1$ marks.**

The side condition $a_i\le n$ says every *gap* of that ruler is at most $n$. Hence

* **A364153(k)** $=$ the minimum, over all Golomb rulers with $k+1$ marks, of the
  **largest gap**;
* **A364132(k)** $=$ the same minimum, restricted to rulers whose gaps increase.

This replaces a search over $k$-tuples drawn from $\{1,\dots,n\}$ by a Golomb-ruler
search, where the Sidon condition prunes almost everything immediately.

## Files

| file | what it is |
|---|---|
| `prototype.py` | slow but obviously-correct Python reference; reproduces the published terms |
| `sidon.c` | the fast searcher (DFS over gaps, bitset of realised differences, two prunes) |
| `runseq.sh` | scans `m = 1, 2, 3, …` until a ruler exists, i.e. computes one term |
| `verify.py` | independent checker: expands all $k(k+1)/2$ consecutive sums of a witness and tests distinctness |
| `results.md` | the computed values, with witnesses |

The decisive prune: the $k(k+1)/2$ consecutive sums are *distinct positive integers
bounded by the total length* $T$, so $T \ge k(k+1)/2$. At every node the search
adds the largest gaps still available; if even that cannot reach $k(k+1)/2$, the
branch dies.

## Reproducing

```sh
gcc -O3 -march=native -o sidon sidon.c
./runseq.sh 14 0 1     # A364153(14): arbitrary order
./runseq.sh 23 1 1     # A364132(23): increasing
python3 verify.py "1,4,6,17,15,14,20,16,19,18,8,13,9,3" 20
```

`./sidon K M mode` exits 10 if a ruler with `K` gaps, all `≤ M`, exists
(mode 0 arbitrary, mode 1 increasing) and 20 if it has proved that none does.
