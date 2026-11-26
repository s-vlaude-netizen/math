#!/usr/bin/env python3
"""DIMACS for: k-colouring of {1..N} in which every 4-term AP has >= 3 distinct colours (Erdos 160 / A402186).
variable x(p,c) = position p has colour c.  Clauses:
  exactly one colour per position; value precedence (colour c may be used at p only if c-1 occurs before p),
  which removes the k! colour symmetry;  for every 4-AP and every pair {a,b} of colours: some term has a colour outside {a,b}.
usage: gen_cnf.py k N > file.cnf"""
import sys, itertools
k,N=int(sys.argv[1]),int(sys.argv[2])
x=lambda p,c:(p-1)*k+c+1            # p in 1..N, c in 0..k-1
cl=[]
for p in range(1,N+1):
    cl.append([x(p,c) for c in range(k)])
    for a,b in itertools.combinations(range(k),2): cl.append([-x(p,a),-x(p,b)])
for p in range(1,N+1):
    for c in range(1,k):
        cl.append([-x(p,c)]+[x(q,c-1) for q in range(1,p)])
for d in range(1,N):
    for a in range(1,N-3*d+1):
        pos=[a,a+d,a+2*d,a+3*d]
        for u,v in itertools.combinations(range(k),2):
            cl.append([x(p,c) for p in pos for c in range(k) if c!=u and c!=v])
print("p cnf %d %d"%(N*k,len(cl)))
for c in cl: print(" ".join(map(str,c)),0)
