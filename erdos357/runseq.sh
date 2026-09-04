#!/bin/bash
# usage: runseq.sh MODE K MSTART
MODE=$2; K=$1; M=$3
while [ $M -le 200 ]; do
  ./sidon $K $M $MODE
  if [ $? -eq 10 ]; then echo "==> a($K) = $M  (mode $MODE)"; exit 0; fi
  M=$((M+1))
done
