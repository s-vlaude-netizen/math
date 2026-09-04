# Erdős Problem 1016 — how sparse can a pancyclic graph be?

<https://www.erdosproblems.com/1016> (status: **open**)

> Let $h(n)$ be minimal such that there is a graph on $n$ vertices with $n+h(n)$
> edges which contains a cycle on $k$ vertices for all $3\le k\le n$
> (such graphs are called *pancyclic*). Estimate $h(n)$. In particular, is it
> true that $h(n)\ge \log_2 n+\log_* n-O(1)$?

Bondy (1971) claimed $\log_2(n-1)-1\le h(n)\le \log_2 n+\log_* n+O(1)$; Erdős
believed the upper bound is closer to the truth but *could not even prove that
$h(n)-\log_2 n\to\infty$*. The values of $n+h(n)$ are
[A105206](https://oeis.org/A105206).

## Why an exhaustive search is possible at all

Two observations shrink the problem from "all graphs on $n$ vertices" to
something a laptop can enumerate:

1. **A pancyclic graph contains an $n$-cycle, so it is Hamiltonian.** Relabelling
   along that cycle, every candidate is $C_n$ plus $k=h$ chords. So instead of
   graphs we enumerate $k$-subsets of the $n(n-3)/2$ chords.

2. **Its cycle space has dimension $m-n+1=k+1$.** Every cycle is a GF(2)
   combination of the $k+1$ fundamental cycles, so the graph has at most
   $2^{k+1}-1$ cycles *in total*, and the complete cycle spectrum is read off
   from at most $2^{k+1}-1$ bitmasks — no cycle enumeration needed.
   (Observation 2 is also exactly Bondy's lower bound: $2^{k+1}-1\ge n-2$.)

Two further ingredients make it fast:

* **Counting prune.** After $j$ chords the remaining $k-j$ can create at most
  $2^{k+1}-2^{j+1}$ further cycles, hence at most that many further lengths. If
  more lengths than that are still missing, the branch dies. This prune gets
  *stronger* as $n$ grows relative to $k$ — the exhaustive $k=4$ refutation takes
  19 s at $n=26$ but 0.2 s at $n=33$.
* **Rotation symmetry.** Every chord set can be rotated so that a chord of
  minimum span $s$ becomes $(0,s)$, all other chords then having span $\ge s$.
  Looping over $s$ and forcing $(0,s)$ covers every set up to rotation and cuts
  the work by a factor of ~21 at $n=25$.

## Files

| file | what it is |
|---|---|
| `pancyclic.c` | the searcher; `./pancyclic n k --canon` exits 10 with a witness or 20 having proved that no $C_n+k$ chords is pancyclic |
| `scan.sh`, `scanrange.sh` | drive `k=0,1,2,…` to get $h(n)$ |
| `verify_pancyclic.py` | independent verification — enumerates *all* simple cycles with networkx instead of using the cycle space |
| `results.md` | the computed values, with witnesses |

`verify_pancyclic.py crosscheck` compares the two spectrum computations on 4000
random graphs; `bruteforce` re-derives small values with a completely separate
exhaustive search.

## Reproducing

```sh
gcc -O3 -march=native -o pancyclic pancyclic.c
./scan.sh 30 6
python3 verify_pancyclic.py witness 25 "(0,2),(0,3),(0,4),(0,20),(1,16)"
python3 verify_pancyclic.py crosscheck
```
