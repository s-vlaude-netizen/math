#!/bin/bash
# runk.sh K NMIN PARTS WORKERS OUTDIR
# Complete search for pancyclic cycle-plus-K-chords graphs with n >= NMIN:
#   gentypes K --minF NMIN-3 --part i PARTS | arcdfs2 NMIN --print     for i = 0..PARTS-1, WORKERS at a time.
# (A type with fewer than NMIN-3 different cycles cannot have n >= NMIN.)
# Resumable: a part whose result file exists is skipped.  Result: OUTDIR/part_<i>.txt
# (solution lines "S ..." and the summary lines "# ..."; the per-type "T" lines are dropped).
cd "$(dirname "$0")"
K=$1; NMIN=$2; M=$3; W=$4; OUT=$5
mkdir -p "$OUT"
for i in $(seq 0 $((M - 1))); do [ -s "$OUT/part_$i.txt" ] || echo $i; done |
  xargs -P "$W" -I{} sh -c "./gentypes $K --minF $((NMIN - 3)) --part {} $M | ./arcdfs2 $NMIN --print | grep -v '^T' > $OUT/part_{}.tmp && mv $OUT/part_{}.tmp $OUT/part_{}.txt"
done=$(ls "$OUT" | grep -c '^part_[0-9]*\.txt$')
echo "$done of $M parts done; types searched: $(cat $OUT/part_*.txt | grep '^# types' | sed 's/.*searched=//; s/ .*//' | paste -sd+ | bc); solutions: $(cat $OUT/part_*.txt | grep -c '^S'); largest n: $(cat $OUT/part_*.txt | grep '^S' | awk '{print $3}' | sort -n | tail -1)"
