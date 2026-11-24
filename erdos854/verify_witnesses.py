#!/usr/bin/env python3
"""Check every witness line 'W n g s_2 s_3 ...' written by gaps2 (WIT=1): build x by CRT with x == -s_p (mod p)
(unused primes, s_p = -1, get any residue not in {0, g mod p}), then verify with real gcds that x and x+g are coprime
to prime(n)# and every x+i, 0<i<g, is not.  So g is a difference of two consecutive coprime numbers."""
import sys
from math import gcd
def primes(m):
    out=[];x=2
    while len(out)<m:
        if all(x%d for d in range(2,int(x**.5)+1)): out.append(x)
        x+=1
    return out
bad=0;cnt=0
for line in open(sys.argv[1]):
    t=line.split()
    if not t or t[0]!='W': continue
    n,g=int(t[1]),int(t[2]); s=list(map(int,t[3:])); P=primes(n); assert len(s)==n
    x=0; M=1
    for p,r in zip(P,s):
        if r<0: r=next(r for r in range(1,p) if r!=g%p)
        a=(-r)%p
        # CRT step
        while x%p!=a: x+=M
        M*=p
    Pn=M
    ok = gcd(x,Pn)==1 and gcd(x+g,Pn)==1 and all(gcd(x+i,Pn)>1 for i in range(1,g))
    cnt+=1
    if not ok: bad+=1; print("BAD",n,g)
print(cnt,"witnesses checked,",bad,"bad")
