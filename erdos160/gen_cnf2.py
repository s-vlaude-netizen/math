#!/usr/bin/env python3
"""Direct encoding for Erdos 160 / A402186: a 4-AP may not use <= 2 colours, so forbid every colour tuple
(c1,c2,c3,c4) with at most two distinct values by a 4-literal clause.  Same position/symmetry clauses as gen_cnf.py.
usage: gen_cnf2.py k N > file.cnf"""
import sys, itertools
k,N=int(sys.argv[1]),int(sys.argv[2])
x=lambda p,c:(p-1)*k+c+1
cl=[]
for p in range(1,N+1):
    cl.append([x(p,c) for c in range(k)])
    for a,b in itertools.combinations(range(k),2): cl.append([-x(p,a),-x(p,b)])
for p in range(1,N+1):
    for c in range(1,k):
        cl.append([-x(p,c)]+[x(q,c-1) for q in range(1,p)])
bad=[t for t in itertools.product(range(k),repeat=4) if len(set(t))<=2]
for d in range(1,N):
    for a in range(1,N-3*d+1):
        pos=[a,a+d,a+2*d,a+3*d]
        for t in bad: cl.append([-x(pos[i],t[i]) for i in range(4)])
print("p cnf %d %d"%(N*k,len(cl)))
for c in cl: print(" ".join(map(str,c)),0)
