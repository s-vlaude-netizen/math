#!/usr/bin/env python3
"""Step 2: pull the OEIS record for every sequence found in step 1."""
import json, os, subprocess, time

HERE = os.path.dirname(os.path.abspath(__file__))
seqs = json.load(open(os.path.join(HERE, "openseqs.json")))
out = {}
for i, s in enumerate(seqs):
    r = subprocess.run(["curl", "-sS", "--max-time", "30",
                        f"https://oeis.org/search?q=id:{s}&fmt=json"],
                       capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
        out[s] = d[0] if isinstance(d, list) else d["results"][0]
    except Exception as e:
        out[s] = {"error": str(e)}
    time.sleep(0.15)
    if i % 50 == 0:
        print(i, flush=True)
json.dump(out, open(os.path.join(HERE, "oeisdata.json"), "w"))
print("done", len(out), "errors", sum(1 for v in out.values() if not v or "error" in v))
