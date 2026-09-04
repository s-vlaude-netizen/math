#!/bin/bash
# psidon.sh K M mode [workers] -- exhaustive search split over the first gap
set -u
K=$1; M=$2; MODE=$3; W=${4:-4}
out=$(mktemp)
seq 1 $M | xargs -P $W -I{} bash -c './sidon2 '"$K $M $MODE"' {}' | tee "$out" >/dev/null
tot=$(awk '{for(i=1;i<=NF;i++) if($i ~ /^nodes=/){split($i,a,"="); s+=a[2]}} END{print s}' "$out")
if grep -q FOUND "$out"; then
  echo "K=$K M=$M mode=$MODE  nodes=$tot  FOUND $(grep -m1 FOUND "$out" | sed 's/.*gaps=/gaps=/')"
  exit 10
fi
echo "K=$K M=$M mode=$MODE  nodes=$tot  NONE (all $M first-gap slices refuted)"
exit 20
