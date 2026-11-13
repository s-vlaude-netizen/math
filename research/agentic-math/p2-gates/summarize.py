#!/usr/bin/env python3
"""Turn results/*.json into the tables of the write-up (markdown on stdout)."""
import glob
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
GATES = ["A", "G1", "G2", "G3a", "G3b", "G4", "G5a", "G5b"]
PRED = {1: "U", 2: "U", 3: "U", 4: "U", 5: "S", 7: "U", 8: "U", 9: "U", 10: "U", 11: "U", 12: "U", 13: "U",
        14: "U", 15: "U", 16: "S", 17: "S*", 18: "U", 19: "U", 20: "A", 21: "U", 22: "U", 23: "U", 24: "U",
        25: "U", 26: "U", 27: "S*", 28: "U", 29: "U", 30: "S", 31: "U", 33: "U", 34: "S", 36: "U", 37: "U",
        38: "U", 39: "S", 41: "U", 46: "U", 47: "U"}
EDIT = {1: "minimum cycle length 3 → 4", 2: "connectivity walk skipped", 3: "fundamental cycle one edge too long",
        4: "closing edge has wrong endpoints", 5: "largest length N → N−1", 7: "no undo after a look-ahead cut",
        8: "count prune `>` → `>=`", 9: "count prune total 2^(K+1) → 2^K", 10: "count prune level j+1 → j+2",
        11: "look-ahead `>` → `>=`", 12: "look-ahead remaining −1", 13: "look-ahead ignores later chords",
        14: "span loop `<=` → `<`", 15: "candidates `>= s` → `> s`", 16: "span without wrap-around",
        17: "forced chord stays a candidate", 18: "mirror, first chord `<` → `<=`", 19: "mirror, later chords `<` → `<=`",
        20: "mirror axis wrong", 21: "mirror table stale after first span", 22: "mirror threshold at wrong depth",
        23: "next start ci+2", 24: "loop stops one early", 25: "search starts at candidate 1",
        26: "chords from i+3", 27: "cycle edge offered as chord", 28: "filter path L−2", 29: "filter path L",
        30: "filter: walks, not paths", 31: "filter forgets first chord", 33: "filter one level early",
        34: "filter never targets length 3", 36: "filter second target unconditional", 37: "slice modulus m+1",
        38: "no K = 0 case", 39: "mirror threshold never set", 41: "state not rebuilt between spans",
        46: "32-bit shift for edge bit", 47: "filter table cut to 32 bits"}
EQUIVALENT = {5, 30, 34, 39}          # no gate fired and the edit is sound by argument
BENIGN = {17, 27}                     # wrong output or state, but no verdict of a canonical search can change

rows = {}
for f in glob.glob(os.path.join(HERE, "results", "m*.json")):
    r = json.load(open(f))
    if r["mut"]:
        rows[r["mut"]] = r


def killed(r, g):
    return r["A"] if g == "A" else not r[g]["ok"]


def matrix():
    print("| id | edit | pred. | " + " | ".join(GATES) + " | lost / bogus in G2 |")
    print("|---:|---|:---:|" + ":---:|" * len(GATES) + "---|")
    for m in sorted(rows):
        r = rows[m]; g2 = r["G2"]
        if not g2["unsliced_ok"]:
            note = f"{g2['lost']:,} / {g2['bogus']:,}"
        elif g2["sliced_bad"]:
            note = "sliced run only"
        else:
            note = ""
        cells = " | ".join("✕" if killed(r, g) else "·" for g in GATES)
        print(f"| {m} | {EDIT[m]} | {PRED[m]} | {cells} | {note} |")


def rates():
    defective = [m for m in rows if m not in EQUIVALENT and m not in BENIGN]
    n = len(defective)
    print(f"\nDefective mutants (can change a verdict or crash): **{n}** of {len(rows)}; "
          f"equivalent: {sorted(EQUIVALENT)}; benign: {sorted(BENIGN)}.\n")
    print("| gate | kills | share |")
    print("|---|---:|---:|")
    for g in GATES:
        k = sum(killed(rows[m], g) for m in defective)
        print(f"| {g} | {k} | {100 * k / n:.0f}% |")
    for name, gs in (("G5a or G5b", ["G5a", "G5b"]), ("G1 or G2", ["G1", "G2"]), ("any gate", GATES)):
        k = sum(any(killed(rows[m], g) for g in gs) for m in defective)
        print(f"| {name} | {k} | {100 * k / n:.0f}% |")
    surv = [m for m in defective if not killed(rows[m], "G1") and not rows[m]["A"]]
    print(f"\nDefective mutants that reproduce all 20 published terms: **{len(surv)}** — {surv}")
    for g in ("G2", "G3a", "G3b", "G4", "G5a", "G5b"):
        print(f"- of these, {g} kills {[m for m in surv if killed(rows[m], g)]}")
    alive = [m for m in defective if not any(killed(rows[m], g) for g in GATES)]
    print(f"- killed by no gate: {alive}")
    return surv


def losses(surv):
    H = {4: 1, 5: 1, 6: 2, 7: 2, 8: 2, 9: 3, 10: 3, 11: 3, 12: 3, 13: 3, 14: 3, 15: 4, 16: 4}
    ref = lambda n, k: int(open(os.path.join(HERE, f"refs/{n}_{k}.txt.done")).read())
    tight = [(n, H[n]) for n in H]
    tot = sum(ref(n, k) for n, k in tight)
    beyond = {}
    p = os.path.join(HERE, "results", "beyond_published.json")
    if os.path.exists(p):
        beyond = json.load(open(p))
    print(f"\n| id | edit | solutions lost, whole grid | lost where k = h(n) ({tot:,} sets) | worst such cell | answers for n = 23..36 |")
    print("|---:|---|---:|---:|---|---|")
    for m in surv:
        g = rows[m]["G2"]
        cells = {(c[0], c[1]): c for c in g["cells"] if len(c) > 2 and isinstance(c[2], int)}
        lt = sum(cells[c][2] for c in tight if c in cells)
        worst = max(((cells[c][2] / ref(*c), c) for c in tight if c in cells and ref(*c)), default=(0, None))
        b = beyond.get(str(m))
        if b is None:
            bt = "not run"
        else:
            wrong = {}
            for n, v in b.items():
                if v != "found":
                    wrong.setdefault(v, []).append(int(n))
            bt = "; ".join(f"{v} at n = {', '.join(map(str, ns))}" for v, ns in wrong.items()) or "all correct"
        wtxt = f"{100 * worst[0]:.0f}% at ({worst[1][0]},{worst[1][1]})" if worst[1] else "—"
        print(f"| {m} | {EDIT[m]} | {100 * g['lost'] / g['ref_total']:.2f}% | {100 * lt / tot:.2f}% | {wtxt} | {bt} |")


if __name__ == "__main__":
    matrix()
    s = rates()
    losses(s)
