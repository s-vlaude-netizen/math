import sys
sys.setrecursionlimit(10000)

def search(n, m, increasing=False):
    """Exists sequence (s_1..s_n), s_i in {1..m}, all contiguous-segment sums distinct?
       <=> prefix sums 0=P_0<...<P_n form a Sidon set (Golomb ruler) with all gaps <= m.
       increasing=True: gaps strictly increasing."""
    marks=[0]
    used=set()
    res=[None]
    def dfs(last, k, mingap):
        if k==n:
            res[0]=list(marks); return True
        lo = mingap if increasing else 1
        for g in range(lo, m+1):
            p = last+g
            newd=[]
            ok=True
            for q in marks:
                d=p-q
                if d in used or d in newd: ok=False;break
                newd.append(d)
            if not ok: continue
            for d in newd: used.add(d)
            marks.append(p)
            if dfs(p, k+1, g+1 if increasing else 1): return True
            marks.pop()
            for d in newd: used.discard(d)
        return False
    return res[0] if dfs(0,0,1) else None

def a(n, increasing):
    m=1
    while True:
        r=search(n,m,increasing)
        if r is not None: return m,r
        m+=1

print("A364153 (arbitrary order):")
vals=[]
for n in range(1,12):
    m,marks=a(n,False)
    gaps=[marks[i+1]-marks[i] for i in range(len(marks)-1)]
    vals.append(m); print(f"  n={n}: a={m}  gaps={gaps}")
print("  computed:", vals)
print("  OEIS    : [1,2,3,5,6,7,9,10,12,13,14]")
print()
print("A364132 (increasing):")
vals=[]
for n in range(1,13):
    m,marks=a(n,True)
    gaps=[marks[i+1]-marks[i] for i in range(len(marks)-1)]
    vals.append(m); print(f"  n={n}: a={m}  gaps={gaps}")
print("  computed:", vals)
print("  OEIS    : [1,2,4,5,7,10,12,13,15,18,21,24]")
