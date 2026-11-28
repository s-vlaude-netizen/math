# How the two problems were picked

Rather than guessing, the choice was made by mining the community database of
Erdős problems and cross-referencing it with the OEIS.

```
python3 01_parse_erdos_db.py     # 1217 problems, 604 open, 214 open ones carry an OEIS sequence
python3 02_fetch_oeis.py         # pull the 286 distinct OEIS records
python3 03_rank_candidates.py    # keep those with keyword "more", sort by #known terms
```

The ranking answers one question: *for which open Erdős problem is the
computational frontier closest?*  A sequence tagged `more` with only a handful
of known terms is one whose next value nobody has computed.

From that shortlist two problems were selected where the next term looked
genuinely reachable — not because the underlying conjecture is easy, but
because each has a reformulation that collapses the search space:

| Erdős # | OEIS | why it looked reachable |
|---|---|---|
| [357](https://www.erdosproblems.com/357) | A364132, A364153 | "all consecutive sums distinct" is exactly the Sidon/Golomb-ruler condition on the partial sums |
| [1016](https://www.erdosproblems.com/1016) | A105206 | a pancyclic graph is Hamiltonian, so it is C_n + k chords, and its cycle space has dimension only k+1 |

Both reformulations turn an apparently unbounded search into a finite one small
enough for a single machine.  See `../erdos357/` and `../erdos1016/`.

## Refresh on 2026-11-28

The three scripts were run again (1221 problems, 563 open, 238 open ones with an OEIS sequence, 326 distinct sequences, 78 of
them tagged `more`); the output is `ranking_2026-11-28.txt`. Many entries are new (A394563, A399654, A398541, A392185, ...).
Looked at for a reachable next term, and not pursued:

* A399654 (cycle-length sets of graphs, 13 terms, n up to 12): the next term needs the cycle spectra of all 2-connected
  graphs on 13 vertices, far more than can be enumerated here.
* A394563/A394564 (Erdős 82, 5 terms, a(6) >= 21): an exact value needs a Ramsey-type proof over graphs on 21+ vertices.
* A245762 (C4-free subgraphs of the n-cube): 304 <= a(7) <= 308, a SAT/ILP-scale proof of the upper bound.
* A362137 (Paley tournaments, a(8)): checking the property for a candidate prime needs a search over about 1e16 orbits of 7-sets.
* A001661, A375077, A388850, A343507, A377587, A119242: terms already near 1e12-1e16; the next ones need a thousand times more.
* A280992: further terms would be products of consecutive primes that are triangular; heuristically none exist, a longer
  search would only give a comment.

The ones already worked on (A389839, A401861, A402186, A105206, A364132/A364153, A399720) are in `../SUBMISSIONS.md` and `../HANDOFF.md`.
