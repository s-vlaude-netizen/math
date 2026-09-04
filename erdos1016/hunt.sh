#!/bin/bash
# hunt.sh n k seconds_per_seed nseeds
n=$1; k=$2; secs=$3; ns=$4
for i in $(seq 1 $ns); do
  s=$((RANDOM*32768+RANDOM+1))
  out=$(./heur $n $k $secs $s)
  if [ $? -eq 10 ]; then echo "$out"; exit 10; fi
done
echo "n=$n k=$k NO WITNESS after $ns x ${secs}s"
