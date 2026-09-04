import sys, itertools
def verify(gaps, claim_m=None, increasing=False):
    n=len(gaps)
    sums=[]
    for u in range(n):
        s=0
        for v in range(u,n):
            s+=gaps[v]; sums.append(s)
    ok_distinct = len(sums)==len(set(sums))
    ok_range = all(1<=g<= (claim_m if claim_m else max(gaps)) for g in gaps)
    ok_incr = all(gaps[i]<gaps[i+1] for i in range(n-1)) if increasing else True
    return ok_distinct, ok_range, ok_incr, len(sums), max(gaps)
if __name__=="__main__":
    g=[int(x) for x in sys.argv[1].split(',')]
    m=int(sys.argv[2]); inc=len(sys.argv)>3 and sys.argv[3]=='inc'
    d,r,i,ns,mx=verify(g,m,inc)
    print(f"k={len(g)} max_gap={mx} (<= {m}: {r})  #segment sums={ns}  all distinct: {d}  increasing ok: {i}")
    assert d and r and i, "INVALID"
    print("VALID")
