#!/usr/bin/env python3
"""
hidden.py N : for every Boolean function of N variables (N <= 4), the least number k of hidden threshold neurons of a
one-hidden-layer network (hidden neurons read the inputs, the output neuron reads only the hidden neurons, all neurons are
linear threshold units) that computes it.  Prints the number of functions needing exactly k.

k = 1 : f is a threshold function.   k = 2 : f or its complement is the AND of two threshold functions.
k >= 3: exact decision by CP-SAT (hidden weights |w| <= 3, which realizes every threshold function of <= 4 variables;
        output weights bounded by the same kind of bound), once per class of functions under permuting/negating inputs and
        negating the output (all three preserve the least k).
"""
import sys, itertools
import numpy as np
from ortools.sat.python import cp_model

N = int(sys.argv[1]); P = 2 ** N; M = (1 << P) - 1
thr = np.fromfile(f"thr_{N}.u32", dtype=np.uint32).astype(np.uint64)
thrset = set(int(h) for h in thr)
A = set(int(x) for x in np.unique(np.bitwise_and.outer(thr, thr)))
T2 = A | {M ^ a for a in A}

# --- classes under the group of the cube (permute / negate inputs) and complement
pts = list(range(P))
perms = []
for pi in itertools.permutations(range(N)):
    for neg in range(P):
        perms.append([sum(((x >> pi[i]) & 1) << i for i in range(N)) ^ neg for x in pts])
F = np.arange(1 << P, dtype=np.uint64)
bits = ((F[:, None] >> np.arange(P, dtype=np.uint64)) & np.uint64(1)).astype(np.uint8)          # 2^P x P
w2 = (np.uint64(1) << np.arange(P, dtype=np.uint64))
canon = F.copy()
for pm in perms:
    g = (bits[:, pm].astype(np.uint64) * w2).sum(axis=1)
    canon = np.minimum(canon, np.minimum(g, np.uint64(M) ^ g))
reps, sizes = np.unique(canon, return_counts=True)
print(f"n={N}: {len(reps)} classes of Boolean functions under input permutation/negation and output negation", flush=True)

VMAX = {3: 3, 4: 3, 5: 5}                                                  # output weight bounds (complete for threshold functions of k variables)

def feasible(f, k, limit=600):
    m = cp_model.CpModel()
    W = [[m.NewIntVar(-3, 3, f"w{j}_{i}") for i in range(N)] for j in range(k)]
    T = [m.NewIntVar(-3 * N, 3 * N + 1, f"t{j}") for j in range(k)]
    V = [m.NewIntVar(-VMAX[k], VMAX[k], f"v{j}") for j in range(k)]
    U = m.NewIntVar(-VMAX[k] * k - 1, VMAX[k] * k + 1, "u")
    for x in range(P):
        zs = []
        for j in range(k):
            z = m.NewBoolVar(f"z{j}_{x}")
            dot = sum(W[j][i] for i in range(N) if (x >> i) & 1)
            m.Add(dot - T[j] >= 0).OnlyEnforceIf(z)
            m.Add(dot - T[j] <= -1).OnlyEnforceIf(z.Not())
            y = m.NewIntVar(-VMAX[k], VMAX[k], f"y{j}_{x}")
            m.Add(y == V[j]).OnlyEnforceIf(z)
            m.Add(y == 0).OnlyEnforceIf(z.Not())
            zs.append(y)
        if (f >> x) & 1: m.Add(sum(zs) >= U)
        else: m.Add(sum(zs) <= U - 1)
    for j in range(k - 1):                                                  # break the symmetry between hidden units
        m.Add(T[j] <= T[j + 1])
    s = cp_model.CpSolver(); s.parameters.max_time_in_seconds = limit; s.parameters.num_search_workers = 2
    st = s.Solve(m)
    if st in (cp_model.OPTIMAL, cp_model.FEASIBLE):                           # re-evaluate the network by hand
        w = [[s.Value(W[j][i]) for i in range(N)] for j in range(k)]
        t = [s.Value(T[j]) for j in range(k)]; v = [s.Value(V[j]) for j in range(k)]; u = s.Value(U)
        for x in range(P):
            z = [1 if sum(w[j][i] for i in range(N) if (x >> i) & 1) >= t[j] else 0 for j in range(k)]
            assert (sum(v[j] * z[j] for j in range(k)) >= u) == bool((f >> x) & 1), "solver returned a network that does not compute f"
        return True
    return {cp_model.INFEASIBLE: False}.get(st)                                # None = unknown

count = {}
unknown = []
for r, sz in zip(reps.tolist(), sizes.tolist()):
    if r in thrset: k = 1
    elif r in T2 or (M ^ r) in T2: k = 2
    else:
        k = 3
        while True:
            res = feasible(r, k)
            if res is None: unknown.append(r); k = None; break
            if res: break
            k += 1
    count[k] = count.get(k, 0) + sz
    if k is None or k >= 3: print(f"  class rep {r:#06x} (size {sz}) needs k = {k}", flush=True)
print("n=%d: functions needing k hidden neurons:" % N, dict(sorted(count.items(), key=lambda kv: (kv[0] is None, kv[0]))), "total", sum(count.values()))
print("check: k=1 ->", len(thrset), " k=2 ->", len(T2 - thrset))
