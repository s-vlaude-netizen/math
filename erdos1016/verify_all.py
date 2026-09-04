#!/usr/bin/env python3
"""Re-verify every claimed value in certificates.json from scratch."""
import json, subprocess, sys
from verify_pancyclic import is_pancyclic

cert = json.load(open("certificates.json"))
bad = 0
for r in cert["new_terms"]:
    n, h, a = r["n"], r["h"], r["a"]
    chords = [tuple(c) for c in r["chords"]]
    ok, sp = is_pancyclic(n, chords)                       # independent: all simple cycles
    edges = n + len(chords)
    if not ok or edges != a or len(chords) != h:
        print(f"  n={n}: WITNESS FAILED"); bad += 1; continue
    # and re-run the refutation of h-1 chords
    rc = subprocess.run(["./pancyclic2", str(n), str(h - 1), "--canon"],
                        capture_output=True, text=True).returncode
    if rc != 20:
        print(f"  n={n}: refutation of k={h-1} FAILED"); bad += 1; continue
    print(f"  n={n}: h={h}, a(n)={a}  witness ok, k={h-1} refuted")
print("ALL OK" if not bad else f"{bad} FAILURES")
sys.exit(1 if bad else 0)
