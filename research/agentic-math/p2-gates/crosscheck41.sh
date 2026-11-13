#!/bin/bash
# Re-decide n=41, k=5 with one rule switched off at a time (resumable, one line per span).
cd "$(dirname "$0")"
opt=$1; first=${2:-2}; LOG=decide/41_5${opt//-/_}.log; touch "$LOG"
for s in $(seq 20 -1 $first); do
  grep -q "^span=$s " "$LOG" && continue
  out=$(./bin/m0 41 5 --span $s $opt); echo "span=$s rc=$? $out" >> "$LOG"
done
