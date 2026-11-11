#!/bin/bash
# Build the naive reference solution sets for the G2 grid (regenerable; not kept in git).
cd "$(dirname "$0")"; export LC_ALL=C; mkdir -p refs
cells="$@"
for nk in $cells; do
  n=${nk%,*}; k=${nk#*,}
  f=refs/${n}_${k}.txt
  [ -s "$f.done" ] || { ./ref_enum $n $k 2>/dev/null | sort -u > "$f" && wc -l < "$f" > "$f.done"; }
  echo "$n $k $(cat "$f.done")"
done
