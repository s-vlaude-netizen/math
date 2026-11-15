# number of subsets S of the vertices of the n-cube with S = conv(S) ∩ {0,1}^n ("convex" vertex sets)
import numpy as np, sys
n = int(sys.argv[1]); P = 2 ** n; M = (1 << P) - 1
H = np.fromfile(f"thr_{n}.u32", dtype=np.uint32).astype(np.uint64)
S = np.arange(1 << P, dtype=np.uint64)
ok = np.ones(1 << P, dtype=bool)
for v in range(P):                              # S is closed iff every vertex v outside S is cut off by a threshold set containing S
    zero_v = H[((H >> np.uint64(v)) & np.uint64(1)) == 0]    # threshold functions with h(v) = 0
    notin = ((S >> np.uint64(v)) & np.uint64(1)) == 0
    cut = np.zeros(1 << P, dtype=bool)
    for h in zero_v:                              # S ⊆ h
        cut |= ((S & ~h & np.uint64(M)) == 0)
    ok &= ~notin | cut
print(n, int(ok.sum()))
