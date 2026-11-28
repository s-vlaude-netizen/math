# Independent check of a family printed by clique/clique_star: usage: python3 -I verify_family.py <output file> <n>
# (the family is on the second line of the file; every pairwise intersection must be a non-empty arithmetic progression)
import re, sys, itertools
txt = open(sys.argv[1]).read().split("\n")
n = int(sys.argv[2])
fam = [frozenset(int(x) for x in s.split(",")) for s in re.findall(r"\{([0-9,]+)\}", txt[1])]
print("sets:", len(fam), " distinct:", len(set(fam)))
assert all(s and all(1 <= x <= n for x in s) for s in fam), "elements out of range or empty set"
def is_ap(s):
    l = sorted(s)
    if len(l) <= 2:
        return True
    d = l[1] - l[0]
    return all(l[i + 1] - l[i] == d for i in range(len(l) - 1))
bad = 0
for a, b in itertools.combinations(fam, 2):
    i = a & b
    if not i or not is_ap(i):
        bad += 1
print("pairs checked:", len(fam) * (len(fam) - 1) // 2, " pairs whose intersection is empty or not an AP:", bad)
print("RESULT:", "valid family of", len(set(fam)), "sets for n =", n) if bad == 0 and len(set(fam)) == len(fam) else print("INVALID")
