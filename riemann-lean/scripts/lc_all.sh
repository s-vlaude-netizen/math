#!/bin/bash
# Replay the declarations of every module of the compact edition (src/**) through Lean's kernel with leanchecker, one module per process, 4 at a time.
cd /home/user/lsernau/grh-lean/grh-compact/src || exit 2
find . -name '*.lean' | sed 's#^\./##; s#\.lean$##; s#/#.#g' | sort > /home/user/lsernau/checks/modules.txt
echo "modules: $(wc -l < /home/user/lsernau/checks/modules.txt)   start $(date -u +%H:%M:%S)"
: > /home/user/lsernau/checks/lc_results.txt
nice -n 5 xargs -a /home/user/lsernau/checks/modules.txt -P 4 -n 1 /home/user/lsernau/checks/lc_one.sh >> /home/user/lsernau/checks/lc_results.txt
echo "end $(date -u +%H:%M:%S)   OK=$(grep -c '^OK' /home/user/lsernau/checks/lc_results.txt)   FAIL=$(grep -c '^FAIL' /home/user/lsernau/checks/lc_results.txt)"
grep '^FAIL' /home/user/lsernau/checks/lc_results.txt | head -20
