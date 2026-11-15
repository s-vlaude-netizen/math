#!/usr/bin/env python3
"""
thr.py N B : all threshold functions of N variables, [w.x >= t] with integer weights |w_i| <= B,
written as truth tables (bit x of the integer = value at the point with binary digits x) to thr_N.u32/.u64.
The count is compared with A000609: 2, 4, 14, 104, 1882, 94572 (n = 0..5); reaching it proves the weight
bound B was large enough (more weights cannot add functions beyond the true total).
"""
import sys, itertools
import numpy as np

N, B = int(sys.argv[1]), int(sys.argv[2])
A000609 = {1: 4, 2: 14, 3: 104, 4: 1882, 5: 94572}
pts = np.array([[(x >> i) & 1 for i in range(N)] for x in range(2 ** N)], dtype=np.int16)   # 2^N x N
ws = np.array(list(itertools.product(range(-B, B + 1), repeat=N)), dtype=np.int16)           # (2B+1)^N x N
dt = np.uint64 if N == 6 else np.uint32 if N == 5 else np.uint16 if N == 4 else np.uint8
seen = set()
chunk = 20000
bitw = (np.uint64(1) << np.arange(2 ** N, dtype=np.uint64))
for s in range(0, len(ws), chunk):
    dots = ws[s:s + chunk] @ pts.T                                   # rows x 2^N
    srt = np.sort(dots, axis=1)
    cols = [srt[:, r] for r in range(2 ** N)] + [srt[:, -1] + 1]     # thresholds: every distinct value, and above the max
    for t in cols:
        f = (dots >= t[:, None])
        tt = (f.astype(np.uint64) * bitw).sum(axis=1)
        seen.update(tt.tolist())
seen = sorted(seen)
print(f"n={N} B={B}: {len(seen)} threshold functions (A000609: {A000609[N]})  {'COMPLETE' if len(seen) == A000609[N] else 'incomplete or wrong'}")
if len(seen) == A000609[N]:
    np.array(seen, dtype=dt).tofile(f"thr_{N}.bin")
