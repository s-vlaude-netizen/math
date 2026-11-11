#!/usr/bin/env python3
"""Run every gate on one build of pcsearch.c and write results/m<id>.json.

    python3 run_gates.py <mutant id>         (0 = the unmodified program)

Gates (see mutants.md): A assertion/crash, G1 published terms, G2 orbit-differential
enumeration, G3a/G3b spectrum differential (small / large n), G4 witness re-check,
G5a/G5b planted-solution retention (small / target size).  For builds that pass G1 the
verdicts at (23,4) and (24,4), just past the published range, are recorded as well.
"""
import json
import os
import random
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
PUBLISHED = [3, 5, 6, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26]   # A105206(3..22)
GRID = [(n, k) for n in range(4, 14) for k in (1, 2, 3, 4)] + \
       [(14, 1), (14, 2), (14, 3), (14, 4), (15, 4), (16, 4), (9, 5), (10, 5), (11, 5)]
SLICED = [(12, 3), (13, 4)]
PLANT_SMALL = [(8, 2), (10, 3), (12, 3), (14, 4), (16, 4), (12, 5)]
PLANT_LARGE = [(33, 5), (36, 5), (41, 4), (41, 5)]
PLANTS = 4000
ENV = dict(os.environ, LC_ALL="C")


def run(cmd, timeout, stdin=None):
    """Returns (exit code, stdout, timed_out)."""
    try:
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, input=stdin, env=ENV)
        return p.returncode, p.stdout, False
    except subprocess.TimeoutExpired:
        return None, "", True


def sh(script, timeout):
    try:
        p = subprocess.run(["bash", "-c", script], capture_output=True, text=True, timeout=timeout, env=ENV, cwd=HERE)
        return p.returncode, p.stdout, False
    except subprocess.TimeoutExpired:
        return None, "", True


def parse_chords(line):
    if "chords=[" not in line:
        return []
    body = line.split("chords=[", 1)[1].split("]", 1)[0]
    out = []
    for part in body.replace("(", "").split(")"):
        part = part.strip(", ")
        if part:
            u, v = part.split(",")
            out.append((int(u), int(v)))
    return out


def build(mut):
    os.makedirs(os.path.join(HERE, "bin"), exist_ok=True)
    binp = os.path.join(HERE, "bin", f"m{mut}")
    subprocess.run(["gcc", "-O2", f"-DMUT={mut}", "-o", binp, os.path.join(HERE, "pcsearch.c")],
                   check=True, capture_output=True)
    return binp


def gate_g1(binp, res):
    bad, wit = [], []
    for i, n in enumerate(range(3, 23)):
        val, note = None, None
        for k in range(0, 7):
            rc, out, to = run([binp, str(n), str(k), "--canon"], 120)
            if to:
                note = "timeout"; break
            if rc == 10:
                val = n + k; wit.append([n, k, parse_chords(out)]); break
            if rc != 20:
                note = f"crash rc={rc}"
                if rc == 3:
                    res["A"] = True
                break
        if val != PUBLISHED[i]:
            bad.append([n, note if note else f"got {val}, published {PUBLISHED[i]}"])
    return dict(ok=not bad, bad=bad), wit


def gate_g4(wit):
    """Every witness must be a set of k distinct real chords whose graph is pancyclic."""
    bogus, lines = [], []
    for n, k, ch in wit:
        norm = sorted({(min(u, v), max(u, v)) for u, v in ch})
        real = all(0 <= u < v < n and v - u >= 2 and not (u == 0 and v == n - 1) for u, v in norm)
        if len(norm) != k or not real:
            bogus.append([n, k, "not k distinct chords"])
        lines.append(f"{n} " + " ".join(f"{u},{v}" for u, v in ch))
    rc, out, to = run([os.path.join(HERE, "ref_enum"), "--spectrum"], 120, "\n".join(lines) + "\n")
    for (n, k, ch), got in zip(wit, out.splitlines()):
        have = set(int(x) for x in got.split())
        if not all(L in have for L in range(3, n + 1)):
            bogus.append([n, k, "not pancyclic"])
    return dict(ok=not bogus, bogus=bogus)


def compare(binp, n, k, extra, tag, res):
    """Enumerate with the build, close under symmetry, compare with the reference set."""
    ref = f"refs/{n}_{k}.txt"
    tmp = f"/tmp/g2_{os.getpid()}_{tag}.txt"
    parts = " ; ".join(f"timeout 400 {binp} {n} {k} --enum --canon {e} 2>/dev/null || echo FAIL$? >&2" for e in extra)
    script = (f"({parts}) 2>{tmp}.err | ./orbit_close {n} | sort -u > {tmp}; "
              f"echo $(comm -13 {tmp} {ref} | wc -l) $(comm -23 {tmp} {ref} | wc -l) $(grep -c FAIL {tmp}.err); "
              f"grep -q FAIL3 {tmp}.err && echo ASSERT; rm -f {tmp} {tmp}.err")
    rc, out, to = sh(script, 900)
    if to:
        return None
    first = out.split("\n")[0].split()
    if "ASSERT" in out:
        res["A"] = True
    return int(first[0]), int(first[1]), int(first[2])


def gate_g2(binp, res):
    lost = bogus = fails = 0
    cells = []
    for n, k in GRID:
        r = compare(binp, n, k, [""], f"{n}_{k}", res)
        if r is None:
            fails += 1; cells.append([n, k, "timeout"]); continue
        lost += r[0]; bogus += r[1]; fails += r[2]
        if r[0] or r[1] or r[2]:
            cells.append([n, k, r[0], r[1], "fail" if r[2] else ""])
    sliced_bad = []
    for n, k in SLICED:
        r = compare(binp, n, k, [f"--slice {i} 3" for i in range(3)], f"s{n}_{k}", res)
        if r is None or r[0] or r[1] or r[2]:
            sliced_bad.append([n, k, r])
    total = sum(int(open(os.path.join(HERE, f"refs/{n}_{k}.txt.done")).read()) for n, k in GRID)
    return dict(ok=not (lost or bogus or fails or sliced_bad), lost=lost, bogus=bogus, fails=fails,
                ref_total=total, cells=cells, unsliced_ok=not (lost or bogus or fails), sliced_bad=sliced_bad)


def gate_g3(binp, lo, hi, cases, seed):
    rnd = random.Random(seed)
    lines = []
    for _ in range(cases):
        n = rnd.randint(lo, hi); k = rnd.randint(0, 6)
        ch = [(u, v) for u in range(n) for v in range(u + 2, n) if not (u == 0 and v == n - 1)]
        lines.append(f"{n} " + " ".join(f"{u},{v}" for u, v in sorted(rnd.sample(ch, min(k, len(ch))))))
    text = "\n".join(lines) + "\n"
    rc1, a, to1 = run([binp, "--spectrum-stdin"], 120, text)
    rc2, b, to2 = run([os.path.join(HERE, "ref_enum"), "--spectrum"], 120, text)
    if to1 or rc1 != 0:
        return dict(ok=False, mismatches=None, note="crash or timeout")
    a, b = a.splitlines(), b.splitlines()
    mism = sum(1 for x, y in zip(a, b) if x != y) + abs(len(a) - len(b))
    return dict(ok=mism == 0, mismatches=mism, cases=cases)


def gate_g5(binp, cells, res):
    lost, per = 0, []
    for n, k in cells:
        rc, out, to = run([binp, str(n), str(k), "--plant", str(PLANTS), "--seed", str(1000 * n + k)], 600)
        if to or rc not in (0, 1):
            if rc == 3:
                res["A"] = True
            per.append([n, k, "timeout" if to else f"crash rc={rc}"]); lost += 1; continue
        x = int(out.strip().split("lost=")[1].split()[0])
        lost += x
        if x:
            per.append([n, k, x])
    return dict(ok=lost == 0, lost=lost, plants=PLANTS * len(cells), cells=per)


def frontier(binp):
    out = {}
    for n, k in ((23, 4), (24, 4)):
        rc, o, to = run([binp, str(n), str(k), "--canon"], 900)
        out[f"{n},{k}"] = "timeout" if to else ("found" if rc == 10 else "none" if rc == 20 else f"crash rc={rc}")
    return out


def main():
    mut = int(sys.argv[1])
    binp = build(mut)
    res = dict(mut=mut, A=False)
    t0 = time.time()
    res["G1"], wit = gate_g1(binp, res)
    res["t_G1"] = round(time.time() - t0, 1)
    res["G4"] = gate_g4(wit) if wit else dict(ok=True, bogus=[])
    res["G3a"] = gate_g3(binp, 4, 16, 2000, 11)
    res["G3b"] = gate_g3(binp, 17, 46, 2000, 12)
    res["G5a"] = gate_g5(binp, PLANT_SMALL, res)
    res["G5b"] = gate_g5(binp, PLANT_LARGE, res)
    t1 = time.time()
    res["G2"] = gate_g2(binp, res)
    res["t_G2"] = round(time.time() - t1, 1)
    if res["G1"]["ok"]:
        res["frontier"] = frontier(binp)
    res["t_total"] = round(time.time() - t0, 1)
    os.makedirs(os.path.join(HERE, "results"), exist_ok=True)
    json.dump(res, open(os.path.join(HERE, "results", f"m{mut}.json"), "w"), indent=1)
    flags = " ".join(f"{g}={'ok' if res[g]['ok'] else 'KILL'}" for g in ("G1", "G2", "G3a", "G3b", "G4", "G5a", "G5b"))
    print(f"m{mut}: A={'KILL' if res['A'] else 'ok'} {flags} frontier={res.get('frontier')} ({res['t_total']}s)")


if __name__ == "__main__":
    main()
