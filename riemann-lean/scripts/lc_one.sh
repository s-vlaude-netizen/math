#!/bin/bash
# usage: lc_one.sh Module   -> replays that module's declarations through Lean's kernel with leanchecker; prints one result line
TC=/home/user/lsernau/.elan/toolchains/leanprover--lean4---v4.35.0
LP=$(cat /home/user/lsernau/checks/lean_path.txt)
start=$(date +%s)
env -i HOME=/home/user/lsernau/home PATH=$TC/bin:/usr/bin:/bin LEAN_PATH="$LP" LANG=C.UTF-8 $TC/bin/leanchecker "$1" > /home/user/lsernau/checks/lc_logs/$1.log 2>&1 &
pid=$!
max=0
while kill -0 $pid 2>/dev/null; do
  r=$(awk '/VmRSS/{print $2}' /proc/$pid/status 2>/dev/null)
  [ -n "$r" ] && [ "$r" -gt "$max" ] && max=$r
  sleep 0.4
done
wait $pid; rc=$?
echo "$([ $rc -eq 0 ] && echo OK || echo FAIL_rc=$rc) $1 $(( $(date +%s) - start ))s ${max}kB"
