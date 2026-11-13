#!/usr/bin/env python3
"""
solve.py -- for a fixed number of chords K, find the largest n for which a pancyclic
cycle-plus-K-chords graph exists, WITHOUT searching over n.

For every type (see arcform.py) the cycle lengths are linear forms in the arc lengths, so
"pancyclic on n = sum of arcs vertices" is a small integer program:

    a_i >= lower bound,  n = sum a_i,
    for every L with 3 <= L <= n-1 some form takes the value L.

Each type is given to CP-SAT.  Types with fewer than n0-3 cycles are skipped when only
n >= n0 is of interest, because they cannot supply enough lengths.

    python3 solve.py K                 maximum n over all types (and a witness)
    python3 solve.py K --min N0        only look for n >= N0   (INFEASIBLE everywhere = no such graph)
    python3 solve.py K --exact N       is there a graph with exactly N vertices?
Options: --workers W (default 4), --limit SECONDS per type (default 600), --out FILE (json lines)
"""
import json
import sys
import time
from multiprocessing import Pool

from ortools.sat.python import cp_model

from arcform import gen_types, lower_bounds, cycle_forms, chord_set

ARGS = {}


def solve_type(job):
    idx, p, edges = job
    n_min, n_exact, limit = ARGS.get("min"), ARGS.get("exact"), ARGS.get("limit", 600)
    lo = lower_bounds(p, edges)
    forms = cycle_forms(p, edges)
    F = len(forms)
    ub = F + 3                                   # lengths 3..n-1 need n-3 different cycles
    target = n_exact or n_min or 0
    if ub < max(target, sum(lo)):
        return dict(idx=idx, p=p, edges=edges, forms=F, status="SKIP", n=None)
    if n_exact:
        ub = n_exact
    m = cp_model.CpModel()
    a = [m.NewIntVar(lo[i], ub, f"a{i}") for i in range(p)]
    n = m.NewIntVar(max(sum(lo), target), ub, "n")
    m.Add(sum(a) == n)
    v, vmin = [], []
    for j, (mj, A) in enumerate(forms):
        lj = mj + sum(lo[i] for i in A)
        vj = m.NewIntVar(lj, ub, f"v{j}")
        m.Add(vj == mj + sum(a[i] for i in A))
        v.append(vj); vmin.append(lj)
    per_form = [[] for _ in range(F)]
    for L in range(3, ub):
        lits = []
        for j in range(F):
            if vmin[j] <= L:
                e = m.NewBoolVar(f"e{j}_{L}")
                m.Add(v[j] == L).OnlyEnforceIf(e)
                lits.append(e); per_form[j].append(e)
        if L < target:                           # n > L is certain: the length is required
            m.AddBoolOr(lits)
        else:
            need = m.NewBoolVar(f"need{L}")      # need = False is only allowed when n <= L
            m.Add(n <= L).OnlyEnforceIf(need.Not())
            m.AddBoolOr(lits + [need.Not()])
    for j in range(F):
        if len(per_form[j]) > 1:
            m.AddAtMostOne(per_form[j])
    if not n_exact:
        m.Maximize(n)
    s = cp_model.CpSolver()
    s.parameters.max_time_in_seconds = limit
    s.parameters.num_search_workers = 1
    t0 = time.time()
    st = s.Solve(m)
    res = dict(idx=idx, p=p, edges=edges, forms=F, status=s.StatusName(st), n=None, secs=round(time.time() - t0, 2))
    if st in (cp_model.OPTIMAL, cp_model.FEASIBLE):
        av = [s.Value(x) for x in a]
        nn = sum(av)
        vals = {mj + sum(av[i] for i in A) for mj, A in forms}
        assert all(L in vals for L in range(3, nn)), "solver returned a non-pancyclic assignment"
        res.update(n=nn, arcs=av, chords=chord_set(edges, av))
    return res


def main():
    k = int(sys.argv[1])
    workers, out = 4, None
    i = 2
    while i < len(sys.argv):
        if sys.argv[i] == "--min": ARGS["min"] = int(sys.argv[i + 1]); i += 2
        elif sys.argv[i] == "--exact": ARGS["exact"] = int(sys.argv[i + 1]); i += 2
        elif sys.argv[i] == "--limit": ARGS["limit"] = float(sys.argv[i + 1]); i += 2
        elif sys.argv[i] == "--workers": workers = int(sys.argv[i + 1]); i += 2
        elif sys.argv[i] == "--out": out = sys.argv[i + 1]; i += 2
        else: raise SystemExit(f"unknown option {sys.argv[i]}")
    t0 = time.time()
    types = gen_types(k)
    jobs = [(idx, p, e) for idx, (p, e) in enumerate(types)]
    print(f"k={k}: {len(types)} types generated in {time.time() - t0:.1f}s", flush=True)
    best, counts, f = None, {}, (open(out, "w") if out else None)
    with Pool(workers) as pool:
        for r in pool.imap_unordered(solve_type, jobs, chunksize=1):
            counts[r["status"]] = counts.get(r["status"], 0) + 1
            if f:
                f.write(json.dumps(r) + "\n"); f.flush()
            if r["n"] is not None and (best is None or r["n"] > best["n"]):
                best = r
    print("statuses:", dict(sorted(counts.items())), f"({time.time() - t0:.1f}s)")
    undecided = counts.get("UNKNOWN", 0)
    if best:
        print(f"largest n found: {best['n']}  type p={best['p']} edges={best['edges']} arcs={best['arcs']}")
        print("chords: " + " ".join(f"{u},{v}" for u, v in best["chords"]))
    else:
        print("no type admits a solution" + (f" with n >= {ARGS['min']}" if ARGS.get("min") else ""))
    if undecided:
        print(f"WARNING: {undecided} types hit the time limit; the result is not a proof")


if __name__ == "__main__":
    main()
