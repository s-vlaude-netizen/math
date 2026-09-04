#!/usr/bin/env python3
"""Re-verify every claimed value in certificates.json.

Without arguments only the witnesses are checked (fast).  With --full the
non-existence proofs are re-run too, which takes tens of minutes.
"""
import json, subprocess, sys
from verify import verify

cert = json.load(open("certificates.json"))
full = "--full" in sys.argv
bad = 0
for name, rows in cert["new_terms"].items():
    mode = 1 if name == "A364132" else 0
    for r in rows:
        k, a, seq = r["k"], r["a"], r["seq"]
        d, rng, inc, nsums, mx = verify(seq, a, increasing=(mode == 1))
        ok = d and rng and inc and len(seq) == k and mx == a and nsums == k * (k + 1) // 2
        print(f"  {name}({k}) = {a}: witness {'ok' if ok else 'FAILED'}"
              f"  ({nsums} consecutive sums, all distinct: {d})")
        if not ok:
            bad += 1
        if full:
            for m in r["refuted"]:
                rc = subprocess.run(["./sidon", str(k), str(m), str(mode)],
                                    capture_output=True, text=True).returncode
                print(f"      m={m}: {'refuted' if rc == 20 else 'REFUTATION FAILED'}")
                if rc != 20:
                    bad += 1
print("ALL OK" if not bad else f"{bad} FAILURES")
sys.exit(1 if bad else 0)
