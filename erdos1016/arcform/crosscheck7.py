#!/usr/bin/env python3
"""
crosscheck7.py : independent test of the 7-chord search with a different solver (CP-SAT).
Sample of types (k = 7, at least 104 cycle forms): every type in which arcdfs2 found a solution with n >= 107, plus random
types without one (half of them among the types with >= 170 forms).  For each, CP-SAT decides "is there an arc vector with
n >= 107 that is pancyclic".  Compare with arcdfs2: a disagreement means a bug in one of the two.
    python3 crosscheck7.py sample          write xcheck/k7_cpsat_sample.txt (needs ./gentypes and k7_min107/)
    python3 crosscheck7.py run WORKERS     solve the sample, results appended to xcheck/k7_cpsat.jsonl (resumable)
    python3 crosscheck7.py report          compare
"""
import glob, json, os, random, subprocess, sys
from multiprocessing import Pool

HERE = os.path.dirname(os.path.abspath(__file__)); os.chdir(HERE)
SAMPLE, OUT = "xcheck/k7_cpsat_sample.txt", "xcheck/k7_cpsat.jsonl"
NMIN, LIMIT = 107, 150

def positives():
    pos = set()
    for f in glob.glob("k7_min107/part_*.txt"):
        for line in open(f):
            if line.startswith("S "): pos.add(int(line.split()[1]))
    return pos

def make_sample():
    pos = positives(); rnd = random.Random(12345)
    resA, resB, seenA, seenB, keep = [], [], 0, 0, {}
    p = subprocess.Popen(["./gentypes", "7", "--minF", "104"], stdout=subprocess.PIPE, text=True)
    for line in p.stdout:
        t = line.split(); idx, F = int(t[0]), int(t[2])
        if idx in pos: keep[idx] = line; continue
        if F >= 104:                                         # reservoir sample 150 of all negatives
            seenA += 1
            if len(resA) < 150: resA.append((idx, line))
            elif rnd.randrange(seenA) < 150: resA[rnd.randrange(150)] = (idx, line)
        if F >= 170:                                         # and 150 of the negatives with many forms
            seenB += 1
            if len(resB) < 150: resB.append((idx, line))
            elif rnd.randrange(seenB) < 150: resB[rnd.randrange(150)] = (idx, line)
    for idx, line in resA + resB: keep.setdefault(idx, line)
    with open(SAMPLE, "w") as f:
        for idx in sorted(keep): f.write(keep[idx])
    print(f"{len(pos)} positive types, sample of {len(keep)} types written; negatives with F>=104: {seenA}, F>=170: {seenB}")

def parse(line):
    t = line.split(); idx, p = int(t[0]), int(t[1]); e = t.index("E")
    ed = list(map(int, t[e + 1:]))
    return idx, p, tuple((ed[i], ed[i + 1]) for i in range(0, len(ed), 2))

def work(job):
    import solve
    solve.ARGS["min"] = NMIN; solve.ARGS["limit"] = LIMIT
    return solve.solve_type(job)

def run(workers):
    done = set()
    if os.path.exists(OUT):
        done = {json.loads(l)["idx"] for l in open(OUT) if l.strip()}
    jobs = [parse(l) for l in open(SAMPLE) if int(l.split()[0]) not in done]
    print(f"{len(done)} done, {len(jobs)} to solve", flush=True)
    with Pool(workers) as pool, open(OUT, "a") as f:
        for r in pool.imap_unordered(work, jobs, chunksize=1):
            f.write(json.dumps(dict(idx=r["idx"], status=r["status"], n=r.get("n"), secs=r.get("secs"))) + "\n"); f.flush()

def report():
    pos = positives(); res = [json.loads(l) for l in open(OUT) if l.strip()]
    agree = dis = unk = 0
    for r in res:
        st = r["status"]
        if st in ("FEASIBLE", "OPTIMAL"):
            ok = r["idx"] in pos and r["n"] >= NMIN
            agree += ok; dis += not ok
            if not ok: print("DISAGREE: CP-SAT feasible, arcdfs2 none", r)
        elif st == "INFEASIBLE":
            ok = r["idx"] not in pos; agree += ok; dis += not ok
            if not ok: print("DISAGREE: CP-SAT infeasible, arcdfs2 has solutions", r)
        elif st == "SKIP": agree += r["idx"] not in pos; dis += r["idx"] in pos
        else: unk += 1
    npos = sum(1 for r in res if r["idx"] in pos)
    print(f"{len(res)} types decided/attempted ({npos} positive for arcdfs2): agree {agree}, disagree {dis}, inconclusive (time limit) {unk}")

if __name__ == "__main__":
    {"sample": make_sample, "run": lambda: run(int(sys.argv[2])), "report": report}[sys.argv[1]]()
