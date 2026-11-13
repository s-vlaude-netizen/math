#!/usr/bin/env python3
"""For builds that reproduce all 20 published terms: what do they answer for n = 23..36,
where the true h(n) is known and an independently verified witness exists?
Asks each build the question "is there a pancyclic C_n + h(n) chords?" (true answer: yes)."""
import json, os, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
H = {n: 4 for n in (23, 24)}; H.update({n: 5 for n in range(25, 37)})
def ask(binp, n, k, limit):
    try:
        p = subprocess.run([binp, str(n), str(k), "--canon"], capture_output=True, text=True, timeout=limit)
    except subprocess.TimeoutExpired:
        return "timeout", None
    if p.returncode == 20: return "none", None
    if p.returncode != 10: return f"crash{p.returncode}", None
    body = p.stdout.split("chords=[", 1)[1].split("]", 1)[0]
    ch = [tuple(int(x) for x in part.strip(", ").split(",")) for part in body.replace("(", "").split(")") if part.strip(", ")]
    line = f"{n} " + " ".join(f"{u},{v}" for u, v in ch) + "\n"
    got = subprocess.run([os.path.join(HERE, "ref_enum"), "--spectrum"], input=line, capture_output=True, text=True).stdout.split()
    real = len(set(ch)) == k and all(str(L) in got for L in range(3, n + 1))
    return ("found" if real else "bogus"), ch
PATH = os.path.join(HERE, "results", "beyond_published.json")
LIMIT = int(os.environ.get("BEYOND_LIMIT", "300"))
for m in [int(x) for x in sys.argv[1:]]:
    binp = os.path.join(HERE, "bin", f"m{m}")
    row = {}
    for n in range(23, 37):
        row[str(n)] = ask(binp, n, H[n], LIMIT)[0]
    out = json.load(open(PATH)) if os.path.exists(PATH) else {}
    out[str(m)] = row                       # saved after every build, so an interrupted run keeps its rows
    json.dump(out, open(PATH, "w"), indent=1)
    wrong = [f"{n}:{v}" for n, v in row.items() if v != "found"]
    print(f"m{m:<3} wrong at: {', '.join(wrong) if wrong else '-'}", flush=True)
