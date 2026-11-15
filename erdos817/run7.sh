#!/bin/bash
# a(7) of A399720: test each maximum N separately (resumable; one line per N in run7.log)
cd "$(dirname "$0")"; touch run7.log
for N in $(seq ${1:-419} ${2:-504}); do grep -q "^N=$N " run7.log || echo $N; done |
  xargs -P ${3:-4} -I{} bash -c 'out=$(./ternary 7 504 {} 2>/dev/null); echo "N={} $out" >> run7.log'
