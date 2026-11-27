#!/bin/bash
# usage: ladder7.sh K N_start N_end seeds secs_per_seed seed_offset
# Several copies (different seed_offset) race on the same frontier: for each N, seeds are tried until ANY copy has found and
# verified a K-colouring of 1..N (file found_K_N.out), then all copies move on to N+1.
K=$1; N0=$2; N1=$3; S=$4; T=$5; OFF=${6:-0}
for N in $(seq $N0 $N1); do
  for s in $(seq 1 $S); do
    [ -f found_${K}_${N}.out ] && break
    out=$(./walkw4ap $K $N $T $((s+OFF)) 2>&1)
    if echo "$out" | grep -q FOUND; then
      col=$(echo "$out" | sed -n 2p)
      if python3 verify.py $K "$col" > /dev/null; then [ -f found_${K}_${N}.out ] || { echo "$out" > found_${K}_${N}.out; echo "K=$K N=$N FOUND seed=$((s+OFF)) (verified)"; }; break; fi
    fi
  done
  [ -f found_${K}_${N}.out ] || { echo "K=$K N=$N: no colouring in $S seeds x ${T}s (copy $OFF)"; exit 1; }
done
