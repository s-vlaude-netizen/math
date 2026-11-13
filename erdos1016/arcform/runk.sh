#!/bin/bash
# runk.sh K NMIN PARTS WORKERS OUTDIR
# Complete search for pancyclic cycle-plus-K-chords graphs with n >= NMIN:
#   gentypes K --minF NMIN-3 --part i PARTS | arcdfs2 NMIN --print     for i = 0..PARTS-1, WORKERS at a time.
# (A type with fewer than NMIN-3 different cycles cannot have n >= NMIN.)
# Resumable.  A part counts as done only if its result file ends with the solver's summary line;
# at the end the number of types searched is compared with the number the generator reports.
# Result: OUTDIR/part_<i>.txt (solution lines "S ..." and the summary lines "# ...";
# the per-type "T" lines are dropped).
cd "$(dirname "$0")"
K=$1; NMIN=$2; M=$3; W=$4; OUT=$5
mkdir -p "$OUT"
complete() { [ -s "$1" ] && tail -2 "$1" | grep -q '^# types='; }
for i in $(seq 0 $((M - 1))); do complete "$OUT/part_$i.txt" || echo $i; done |
  xargs -P "$W" -I{} bash -c "set -o pipefail; ./gentypes $K --minF $((NMIN - 3)) --part {} $M | ./arcdfs2 $NMIN --print | grep -v '^T' > $OUT/part_{}.tmp && tail -2 $OUT/part_{}.tmp | grep -q '^# types=' && mv $OUT/part_{}.tmp $OUT/part_{}.txt"
ndone=0; for i in $(seq 0 $((M - 1))); do complete "$OUT/part_$i.txt" && ndone=$((ndone + 1)); done
searched=$(cat $OUT/part_*.txt | grep '^# types' | sed 's/.*searched=//; s/ .*//' | paste -sd+ | bc)
expected=$(./gentypes $K --minF $((NMIN - 3)) --count | sed -n 's/.*cycle forms: \([0-9]*\); most.*/\1/p')
echo "$ndone of $M parts done; types searched: $searched (generator: $expected); solutions: $(cat $OUT/part_*.txt | grep -c '^S'); largest n: $(cat $OUT/part_*.txt | grep '^S' | awk '{print $3}' | sort -n | tail -1)"
[ "$ndone" -eq "$M" ] && [ "$searched" = "$expected" ] && echo "COMPLETE: every type with at least $((NMIN - 3)) cycle forms was searched for n >= $NMIN"
