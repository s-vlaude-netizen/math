#!/usr/bin/env python3
"""Verify a colouring: usage verify.py k "c1 c2 ... cN"  -- every 4-term AP in 1..N must have >= 3 distinct colours."""
import sys
k=int(sys.argv[1]); c=list(map(int,sys.argv[2].split())); N=len(c)
assert all(1<=x<=k for x in c)
bad=[(a,d) for d in range(1,N) for a in range(1,N-3*d+1) if len({c[a-1],c[a+d-1],c[a+2*d-1],c[a+3*d-1]})<3]
print("N=%d colours used=%d violating 4-APs=%d"%(N,len(set(c)),len(bad)))
sys.exit(1 if bad else 0)
