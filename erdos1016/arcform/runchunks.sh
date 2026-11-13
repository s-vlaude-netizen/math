#!/bin/bash
# runchunks.sh BINARY K NMIN CHUNKS WORKERS OUTDIR
# Run BINARY NMIN --print over types_K.txt split into CHUNKS interleaved parts, WORKERS at a time.
# Resumable: a part whose result file exists is skipped.  Result: OUTDIR/part_<i>.txt
cd "$(dirname "$0")"
BIN=$1; K=$2; NMIN=$3; M=$4; W=$5; OUT=$6
mkdir -p "$OUT"
for i in $(seq 0 $((M - 1))); do [ -s "$OUT/part_$i.txt" ] || echo $i; done |
  xargs -P "$W" -I{} sh -c "awk -v m=$M -v i={} 'NR%m==i' types_$K.txt | $BIN $NMIN --print > $OUT/part_{}.tmp && mv $OUT/part_{}.tmp $OUT/part_{}.txt"
done=$(ls "$OUT" | grep -c '^part_[0-9]*\.txt$')
echo "$done of $M parts done; solutions: $(cat $OUT/part_*.txt 2>/dev/null | grep -c '^S')"
