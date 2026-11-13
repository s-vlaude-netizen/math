#!/bin/bash
# hunt6.sh K FROM TO SECONDS -- look for a K-chord pancyclic witness for each n (hill climbing),
# verify each with the independent reference, append to decide/witnesses_k$K.txt
cd "$(dirname "$0")"
K=$1; OUT=decide/witnesses_k$K.txt; touch "$OUT"
for n in $(seq $2 $3); do
  grep -q "^$n $K " "$OUT" && continue
  w=$(timeout $4 ./pcsearch_new $n $K --near 1 $((n-2)) --seed $((n*7+K)))
  if [ -n "$w" ]; then
    ok=$(echo "$n $w" | ./ref_enum --spectrum | awk -v n=$n '{print (NF==n-2 && $1==3 && $NF==n) ? "ok" : "BAD"}')
    [ "$ok" == "ok" ] && echo "$n $K $w" >> "$OUT"
    echo "n=$n k=$K witness ($ok)"
  else echo "n=$n k=$K: none found in $4 s"; fi
done
