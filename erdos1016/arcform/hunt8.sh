#!/bin/bash
# hunt8.sh TYPESFILE OUT : 8-chord witnesses, first solution found for each n, window by window.
# (Sampling only: it finds witnesses, it proves nothing about the largest n.)
cd "$(dirname "$0")"
T=$1; OUT=$2; touch "$OUT"
for win in "115 134" "135 154" "155 170" "171 182" "183 192" "193 215"; do
  set -- $win; lo=$1; hi=$2
  have=$(awk -v lo=$lo -v hi=$hi '$1=="S" && $3>=lo && $3<=hi' "$OUT" | wc -l)
  [ "$have" -ge $((hi - lo + 1)) ] && continue
  timeout 1500 ./arcdfs2 $lo --max $hi --print < "$T" |
    awk -v want=$((hi - lo + 1)) '$1=="S" && !($3 in seen) { seen[$3]=1; print; fflush(); c++ } c==want { exit }' >> "$OUT"
done
awk '$1=="S" && !($3 in s) { s[$3]=1 } END { n=0; for (k in s) n++; print n " values of n with a witness" }' "$OUT"
