# Largest n such that 1..n can be k-coloured with every 4-term arithmetic progression using >= 3 colours (k = 1, 2, 3, ...).
# Plain backtracking, colours introduced in order. Fast enough for k <= 4; k = 5 and 6 need the C programs in this directory.
import sys
sys.setrecursionlimit(10000)
def longest(k):
    col = []; best = [0]
    def ok(p):
        return all(len({col[p], col[p-d], col[p-2*d], col[p-3*d]}) >= 3 for d in range(1, p//3 + 1))
    def rec(p, used):
        best[0] = max(best[0], p)
        for c in range(min(used + 1, k)):
            col.append(c)
            if ok(p): rec(p + 1, max(used, c + 1))
            col.pop()
    rec(0, 0)
    return best[0]
print([longest(k) for k in range(1, 5)])   # [3, 3, 12, 22]
