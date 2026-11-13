#!/bin/bash
# cmp_ref.sh K NMAX : all solutions of arcdfs2 with n <= NMAX, closed under the 2n symmetries,
# against the naive reference sets ../../research/agentic-math/p2-gates/refs/N_K.txt (where present).
cd "$(dirname "$0")"; export LC_ALL=C
K=$1; NMAX=$2; G=../../research/agentic-math/p2-gates; T=$(mktemp -d)
./gentypes $K | ./arcdfs2 0 --print --max $NMAX | grep '^S' > $T/sol.txt
bad=0
for n in $(seq 4 $NMAX); do
  ref=$G/refs/${n}_$K.txt
  [ -s "$ref.done" ] || continue
  awk -v n=$n '$3==n { sub(/.* C /, ""); print }' $T/sol.txt | $G/orbit_close $n | sort -u > $T/closed.txt
  if cmp -s $T/closed.txt $ref; then echo "n=$n k=$K: equal ($(wc -l < $T/closed.txt) chord sets)"; else bad=1; echo "n=$n k=$K: MISMATCH solver $(wc -l < $T/closed.txt) reference $(wc -l < $ref)"; fi
done
rm -rf $T; exit $bad
