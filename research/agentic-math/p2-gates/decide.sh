#!/bin/bash
# decide.sh N K [workers] -- exhaustive decision "is there a pancyclic C_N + K chords?"
# One line per minimum span in decide/N_K.log; spans already logged are skipped, so the
# run can be interrupted and resumed.  Uses bin/m0, the build that passed every gate.
cd "$(dirname "$0")"
N=$1; K=$2; W=${3:-4}; LOG=decide/${N}_${K}.log
touch "$LOG"
for s in $(seq 2 $((N / 2))); do grep -q "^span=$s " "$LOG" || echo $s; done |
  xargs -P "$W" -I{} bash -c 'out=$(./bin/m0 '"$N $K"' --span {}); echo "span={} rc=$? $out" >> '"$LOG"
sort -t= -k2n "$LOG" -o "$LOG"
total=$((N / 2 - 1)); none=$(grep -c "rc=20 " "$LOG"); found=$(grep -c "rc=10 " "$LOG")
echo "N=$N K=$K: $none of $total spans refuted, $found with a witness"
if [ "$found" -gt 0 ]; then grep -m1 "rc=10 " "$LOG"; exit 10; fi
[ "$none" -eq "$total" ] && { echo "VERDICT: no pancyclic C_$N plus $K chords exists"; exit 20; }
echo "VERDICT: incomplete"; exit 1
