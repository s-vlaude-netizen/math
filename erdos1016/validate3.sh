#!/bin/bash
# Validate pancyclic3 (look-ahead prune + mirror symmetry) against pancyclic2's verdicts.
cd "$(dirname "$0")"
echo "== all 20 published terms of A105206 must come back =="
for n in $(seq 3 22); do for k in 0 1 2 3 4 5; do ./pancyclic3 $n $k --canon >/dev/null 2>&1; if [ $? -eq 10 ]; then echo -n "$((n+k)) "; break; fi; done; done
echo; echo "3 5 6 8 9 10 12 13 14 15 16 17 19 20 21 22 23 24 25 26  <- known"
echo "== refutations must stay IMPOSSIBLE =="
for nk in "25 4" "26 4" "30 4" "33 4"; do ./pancyclic3 $nk --canon; done
echo "== witnesses must still be found (the hard ones) =="
for nk in "35 5" "36 5" "39 5" "40 5"; do ./pancyclic3 $nk --canon; done
echo "== speed, n=41 span 16 (pancyclic2: 71473621 nodes / 81.7s) =="
./pancyclic3 41 5 --span 16
