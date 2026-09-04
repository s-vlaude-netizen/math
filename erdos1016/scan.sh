#!/bin/bash
# find h(n) = min k such that C_n + k chords can be pancyclic (canonical search)
n=$1; kmax=${2:-8}
for k in $(seq 0 $kmax); do
  out=$(./pancyclic $n $k --canon)
  rc=$?
  if [ $rc -eq 10 ]; then echo "n=$n  h(n)=$k  a(n)=$((n+k))   $out"; exit 0; fi
  echo "   [n=$n k=$k ruled out: $out]"
done
echo "n=$n  NO SOLUTION up to k=$kmax"
