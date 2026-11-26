#!/bin/bash
# usage: solve_col.sh k N [conflict_limit]  -- builds the CNF, runs the SAT solver, verifies any colouring found
k=$1; N=$2; lim=${3:-0}
f=$(mktemp /tmp/claude-0/-home-user/d2b279bd-d4f4-5072-be7f-ccaf61d9a5f6/scratchpad/colXXXXXX.cnf)
python3 "$(dirname "$0")/gen_cnf2.py" $k $N > $f
out=$(/home/user/math/sat/sat $f $lim 2>/dev/null)
if echo "$out" | grep -q "^s SATISFIABLE"; then
  model=$(echo "$out" | grep '^v' | sed 's/^v//')
  col=$(python3 - "$k" "$N" $model <<'PY'
import sys
k,N=int(sys.argv[1]),int(sys.argv[2]); lits=[int(t) for t in sys.argv[3:] if t!='0']
true={l for l in lits if l>0}
print(" ".join(str(next(c for c in range(k) if (p*k+c+1) in true)+1) for p in range(N)))
PY
)
  python3 "$(dirname "$0")/verify.py" $k "$col" >/dev/null && echo "k=$k N=$N SAT (verified): $col" || echo "k=$k N=$N SAT but VERIFICATION FAILED"
elif echo "$out" | grep -q "UNSATISFIABLE"; then echo "k=$k N=$N UNSAT  $(echo "$out" | grep '^c conflicts')"
else echo "k=$k N=$N UNKNOWN $(echo "$out" | grep '^c')"; fi
rm -f $f
