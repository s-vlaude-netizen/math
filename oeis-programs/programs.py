# Python programs for OEIS combinatorics sequences that had none (checked 2026-11-15).
# Each entry: A-number -> source to paste into the PROG field (the function is named after the sequence).
PROGRAMS = {}

PROGRAMS["A054578"] = '''def A054578(n):  # nonempty subsets of {1..n} with all pairwise differences distinct
    def count(lo, diffs, elems):
        c = 0
        for x in range(lo, n + 1):
            d = {x - y for y in elems}
            if not d & diffs:
                c += 1 + count(x + 1, diffs | d, elems + [x])
        return c
    return count(1, set(), [])'''

PROGRAMS["A143824"] = '''def A143824(n):  # largest subset of {1..n} with distinct differences; WLOG it contains 1
    best = 0
    def grow(lo, diffs, elems):
        nonlocal best
        best = max(best, len(elems))
        for x in range(lo, n + 1):
            if len(elems) + n - x + 1 <= best: break
            d = {x - y for y in elems}
            if not d & diffs:
                grow(x + 1, diffs | d, elems + [x])
    if n: grow(2, set(), [1])
    return best'''

PROGRAMS["A241689"] = '''def A241689(n, k=5):  # Sidon subsets of {1..n} of size k
    def count(lo, diffs, elems):
        if len(elems) == k: return 1
        c = 0
        for x in range(lo, n - k + len(elems) + 2):
            d = {x - y for y in elems}
            if not d & diffs:
                c += count(x + 1, diffs | d, elems + [x])
        return c
    return count(1, set(), [])'''

PROGRAMS["A241690"] = PROGRAMS["A241689"].replace("A241689(n, k=5)", "A241690(n, k=6)")

PROGRAMS["A196723"] = '''def A196723(n):  # subsets of {1..n} whose sums of two distinct elements are all distinct
    def count(lo, sums, elems):
        c = 1
        for x in range(lo, n + 1):
            s = {x + y for y in elems}
            if not s & sums:
                c += count(x + 1, sums | s, elems + [x])
        return c
    return count(1, set(), [])'''

PROGRAMS["A085489"] = '''def A085489(n):  # subsets of {1..n} with no x + y = z, x != y; elements are added in increasing order
    def count(lo, sums, elems):
        c = 1
        for z in range(lo, n + 1):
            if z not in sums:
                c += count(z + 1, sums | {z + y for y in elems}, elems + [z])
        return c
    return count(1, set(), [])'''

PROGRAMS["A088809"] = '''def A088809(n):  # 2^n minus the subsets of {1..n} with no x + y = z, x != y (A085489)
    def count(lo, sums, elems):
        c = 1
        for z in range(lo, n + 1):
            if z not in sums:
                c += count(z + 1, sums | {z + y for y in elems}, elems + [z])
        return c
    return 2**n - count(1, set(), [])'''

PROGRAMS["A151897"] = '''def A151897(n):  # subsets of {1..n}, no member a sum of distinct other members; m = bitmask of subset sums
    def count(lo, m):
        c = 1
        for z in range(lo, n + 1):
            if not m >> z & 1:
                c += count(z + 1, m | m << z)
        return c
    return count(1, 1)'''

PROGRAMS["A097514"] = '''from math import comb
def A097514(n):  # the block containing the largest element has size k != 2
    a = [1]
    for m in range(1, n + 1):
        a.append(sum(comb(m - 1, k - 1) * a[m - k] for k in range(1, m + 1) if k != 2))
    return a[n]'''

PROGRAMS["A057814"] = '''from math import comb
def A057814(n):  # the block containing the largest element has size k >= 5
    a = [1]
    for m in range(1, n + 1):
        a.append(sum(comb(m - 1, k - 1) * a[m - k] for k in range(5, m + 1)))
    return a[n]'''

PROGRAMS["A025036"] = '''from math import factorial
def A025036(n): return factorial(4 * n) // (factorial(n) * 24**n)'''

PROGRAMS["A000356"] = '''from math import factorial
def A000356(n): return factorial(2 * n) * factorial(2 * n + 1) // (factorial(n)**2 * factorial(n + 1) * factorial(n + 2))'''

PROGRAMS["A005514"] = '''from math import comb, gcd
from sympy import divisors, totient
def A005514(n, k=8):  # Burnside: necklaces N, reflections fix comb(floor(n/2), k/2) colorings on average (k even)
    N = sum(totient(d) * comb(n // d, k // d) for d in divisors(gcd(n, k))) // n
    return (N + comb(n // 2, k // 2)) // 2'''

PROGRAMS["A036501"] = '''def A036501(n):  # rulers with marks 0 and L counted, then halved: for n >= 3 no Golomb ruler is symmetric
    if n == 2: return 1
    L = 0
    while True:
        L += 1
        def count(lo, diffs, elems):
            if len(elems) == n: return 1
            c = 0
            for x in range(lo, L):
                d = {abs(x - y) for y in elems}
                if len(d) == len(elems) and not d & diffs:
                    c += count(x + 1, diffs | d, elems + [x])
            return c
        t = count(1, {L}, [0, L])
        if t: return t // 2'''
