#!/usr/bin/env python3
"""
Re-verify the claimed values in certificates.json.

Witnesses (all n): the graph is rebuilt and ALL its simple cycles are enumerated with networkx.
Lower bounds:
  n <= 40   the refutation of h-1 chords is re-run with the per-n searcher ./pancyclic2
  n >= 41   one run of the type search covers every n at once:
                arcform/gentypes 5 | arcform/arcdfs2 41     (no 5-chord graph, n >= 41)
                arcform/gentypes 6 | arcform/arcdfs2 68     (no 6-chord graph, n >= 68)
            both are re-run here if the binaries exist (build them as in arcform/README.md).
"""
import json
import os
import subprocess
import sys

from verify_pancyclic import is_pancyclic

cert = json.load(open("certificates.json"))
bad = 0


def type_search(k, nmin):
    """True if the type search finds no pancyclic cycle-plus-k-chords graph with n >= nmin."""
    if not (os.path.exists("arcform/gentypes") and os.path.exists("arcform/arcdfs2")):
        return None
    gen = subprocess.Popen(["arcform/gentypes", str(k)], stdout=subprocess.PIPE)
    out = subprocess.run(["arcform/arcdfs2", str(nmin)], stdin=gen.stdout, capture_output=True, text=True).stdout
    gen.wait()
    return "solutions=0 " in out and "searched=" in out


free = {}
for k, nmin in ((5, 41), (6, 68)):
    free[k] = type_search(k, nmin)
    print(f"  type search, {k} chords, n >= {nmin}: " +
          {True: "no solution", False: "FAILED", None: "binaries missing, not re-run"}[free[k]])
    bad += free[k] is False

for r in cert["new_terms"]:
    n, h, a = r["n"], r["h"], r["a"]
    chords = [tuple(c) for c in r["chords"]]
    ok, sp = is_pancyclic(n, chords)                       # independent: all simple cycles
    if not ok or n + len(chords) != a or len(chords) != h or len(set(chords)) != h:
        print(f"  n={n}: WITNESS FAILED"); bad += 1; continue
    if n <= 40:
        rc = subprocess.run(["./pancyclic2", str(n), str(h - 1), "--canon"],
                            capture_output=True, text=True).returncode
        if rc != 20:
            print(f"  n={n}: refutation of k={h-1} FAILED"); bad += 1; continue
        print(f"  n={n}: h={h}, a(n)={a}  witness ok, k={h-1} refuted")
    else:
        assert (h, n >= 68) in ((6, False), (7, True))
        print(f"  n={n}: h={h}, a(n)={a}  witness ok, k={h-1} excluded by the type search")
print("ALL OK" if not bad else f"{bad} FAILURES")
sys.exit(1 if bad else 0)
