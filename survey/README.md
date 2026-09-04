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
