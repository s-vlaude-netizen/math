# Check that every root of every share of the n = 13 search, and every child of the split root 0, is covered by a finished list.
# usage: python3 -I check_n13_coverage.py <erdos272 directory>   (reads n13_lb83_part*.ckpt, n13b_w*.ckpt, n13b_w*.ckpt1, n13b_w*.err)
import sys, re
d = sys.argv[1]
def load(f):
    try:
        return [int(x) for x in open(f).read().split()]
    except FileNotFoundError:
        return []
old = {}
for name in ["n13_lb83_part0.ckpt", "n13_lb83_part1.ckpt", "n13_lb83_part2.ckpt", "n13_lb83_root0.ckpt"]:
    old[name] = load(f"{d}/{name}")
new = {r: load(f"{d}/n13b_w{r}.ckpt") for r in range(4)}
ck1 = {r: load(f"{d}/n13b_w{r}.ckpt1") for r in range(4)}
print("old lists:", {k: len(v) for k, v in old.items()}, " max:", {k: (max(v) if v else None) for k, v in old.items()})
print("new lists:", {r: len(v) for r, v in new.items()}, " max:", {r: (max(v) if v else None) for r, v in new.items()})
allold = set(x for v in old.values() for x in v)
allnew = set(x for v in new.values() for x in v)
print("overlap old/new:", sorted(allold & allnew)[:10], "(count %d)" % len(allold & allnew))
# class check of the new lists
for r, v in new.items():
    bad = [x for x in v if x % 4 != r]
    print(f"worker {r}: roots not congruent to {r} mod 4 in its own list: {bad[:5]} (count {len(bad)})")
# final root index per finished worker from the .err logs
ends = {}
for r in range(4):
    last = None
    for ln in open(f"{d}/n13b_w{r}.err"):
        m = re.match(r"root (\d+)/(\d+) done", ln)
        if m: last = int(m.group(1))
    ends[r] = last
print("last root index logged per worker:", ends)
done_all = allold | allnew
for r in (0, 1, 2, 3):
    hi = ends[r]
    missing = [k for k in range(1, hi + 1) if k % 4 == r and k not in done_all]
    print(f"worker {r} class: roots <= {hi} in class {r} mod 4 that are in NO done list: {missing[:10]} (count {len(missing)})")
# class 1 (still running): check up to its current progress
hi1 = max(new[1]) if new[1] else 0
missing1 = [k for k in range(1, hi1 + 1) if k % 4 == 1 and k not in done_all]
print(f"worker 1: class-1 roots <= {hi1} in NO done list: {missing1[:10]} (count {len(missing1)})")
# root 0 children
allc = set(x for v in ck1.values() for x in v)
print("root-0 children done per worker:", {r: len(v) for r, v in ck1.items()}, " union size:", len(allc))
if allc:
    top = max(allc)
    gaps = [c for c in range(0, top + 1) if c not in allc]
    print(f"root-0 children: max index {top}; indices below it done by nobody: {gaps[:10]} (count {len(gaps)})")
    bad = [(r, x) for r, v in ck1.items() for x in v if x % 4 != r]
    print("children in the wrong worker class:", bad[:5], "(count %d)" % len(bad))
