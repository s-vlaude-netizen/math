#!/bin/bash
for n in $(seq $1 $2); do ./scan.sh $n ${3:-6}; done
