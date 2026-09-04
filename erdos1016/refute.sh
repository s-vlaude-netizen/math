#!/bin/bash
# refute.sh n k [workers]
#
# Exhaustively decides whether C_n plus k chords can be pancyclic, spreading the
# canonical search over several processes.  The canonical search loops over the
# minimum chord span s and forces the chord (0,s); the small-span branches carry
# almost all the work, so they are additionally split with --slice.
#
# Prints one line per task and a final verdict.  Exit 10 = witness found,
# 20 = proved impossible, 1 = some task failed.
set -u
n=$1; k=$2; W=${3:-4}
half=$((n / 2))
tasks=$(mktemp)
for s in $(seq 2 $half); do
  # more slices for the cheap-to-branch small spans, which dominate the cost
  case $s in
    2) m=24 ;; 3) m=16 ;; 4) m=12 ;; 5) m=8 ;; 6) m=6 ;; 7) m=4 ;; 8) m=3 ;; 9) m=2 ;; *) m=1 ;;
  esac
  for i in $(seq 0 $((m - 1))); do echo "$s $i $m" >> "$tasks"; done
done
echo "$(wc -l < "$tasks") tasks, $W workers"

out=$(mktemp)
xargs -a "$tasks" -n 3 -P "$W" bash -c '
  s=$0; i=$1; m=$2
  r=$(./pancyclic2 '"$n"' '"$k"' --span $s --slice $i $m); rc=$?
  echo "span=$s slice=$i/$m rc=$rc $r"
' | tee "$out"

if grep -q "PANCYCLIC" "$out"; then
  echo "VERDICT: witness found"; exit 10
fi
bad=$(grep -c "rc=20" "$out")
tot=$(wc -l < "$tasks")
if [ "$bad" -eq "$tot" ]; then
  echo "VERDICT: no pancyclic C_$n plus $k chords exists (all $tot tasks refuted)"; exit 20
fi
echo "VERDICT: incomplete ($bad of $tot tasks refuted)"; exit 1
