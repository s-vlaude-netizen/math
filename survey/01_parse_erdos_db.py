#!/usr/bin/env python3
"""Step 1: download the community Erdos-problem database and list the OPEN
problems that have an associated OEIS sequence.

    curl -o problems.yaml https://raw.githubusercontent.com/teorth/erdosproblems/main/data/problems.yaml
"""
import json, os, re, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
YAML = os.path.join(HERE, "problems.yaml")
URL = "https://raw.githubusercontent.com/teorth/erdosproblems/main/data/problems.yaml"

if not os.path.exists(YAML):
    subprocess.run(["curl", "-sS", "--max-time", "180", "-o", YAML, URL], check=True)

txt = open(YAML).read()
probs = []
for b in txt.split("\n- number: "):
    if not b.strip():
        continue
    if not b.startswith("- number:"):
        b = "- number: " + b
    num = re.search(r'- number: "([^"]+)"', b).group(1)
    st = re.search(r'\n  status:\n    state: "([^"]+)"', b)
    oeis = re.findall(r"oeis: \[([^\]]*)\]", b)
    tags = re.findall(r"tags: \[([^\]]*)\]", b)
    probs.append(dict(
        num=num,
        state=st.group(1) if st else "?",
        oeis=re.findall(r'"(A\d+)"', oeis[0]) if oeis else [],
        tags=re.findall(r'"([^"]+)"', tags[0]) if tags else [],
    ))

json.dump(probs, open(os.path.join(HERE, "probs.json"), "w"))
openseq = [p for p in probs if p["state"] == "open" and p["oeis"]]
seqs = sorted({s for p in openseq for s in p["oeis"]})
json.dump(seqs, open(os.path.join(HERE, "openseqs.json"), "w"))

from collections import Counter
print("problems:", len(probs), Counter(p["state"] for p in probs))
print("open problems with an OEIS sequence:", len(openseq))
print("distinct sequences:", len(seqs))
