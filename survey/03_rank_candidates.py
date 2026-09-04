#!/usr/bin/env python3
"""Step 3: rank the sequences of open Erdos problems by how likely it is that a
single machine can push them further.

The heuristic: OEIS keyword "more" means the editors want more terms, and few
known terms means the frontier is close.  Sorting by the number of known terms
puts the reachable frontiers first.
"""
import json, os

HERE = os.path.dirname(os.path.abspath(__file__))
data = json.load(open(os.path.join(HERE, "oeisdata.json")))
probs = json.load(open(os.path.join(HERE, "probs.json")))

seq2prob = {}
for p in probs:
    if p["state"] != "open":
        continue
    for s in p["oeis"]:
        seq2prob.setdefault(s, []).append(p["num"])

rows = []
for s, rec in data.items():
    if not rec or "error" in rec:
        continue
    kw = rec.get("keyword", "")
    if "more" not in kw:
        continue
    n = len([t for t in rec.get("data", "").split(",") if t.strip()])
    rows.append((n, s, rec.get("name", "")[:110], kw, seq2prob.get(s)))

rows.sort()
for n, s, name, kw, pr in rows:
    print(f"{s}  terms={n:3d}  erdos#={pr}  kw={kw}\n     {name}")
print("candidates:", len(rows))
