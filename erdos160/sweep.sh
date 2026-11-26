#!/bin/bash
# usage: sweep.sh N first_seed last_seed seconds_per_seed   -- run the weighted local search with many seeds, stop at the first success
N=$1; s0=$2; s1=$3; T=$4
for s in $(seq $s0 $s1); do
  out=$(./walkw4ap 6 $N $T $s 2>&1)
  if echo "$out" | grep -q FOUND; then echo "$out" > found_6_${N}.out; echo "N=$N seed=$s FOUND"; exit 0; fi
done
echo "N=$N seeds $s0..$s1: none found"
