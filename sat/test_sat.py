#!/usr/bin/env python3
"""Randomised differential test of ./sat against brute force (numpy truth table) on small CNFs."""
import random, subprocess, sys, os, numpy as np, tempfile
random.seed(int(sys.argv[1]) if len(sys.argv)>1 else 1)
def brute(n, clauses):
    idx=np.arange(1<<n, dtype=np.uint32); ok=np.ones(1<<n,dtype=bool)
    for cl in clauses:
        sat=np.zeros(1<<n,dtype=bool)
        for l in cl:
            b=((idx>>(abs(l)-1))&1).astype(bool)
            sat|= b if l>0 else ~b
        ok&=sat
    return bool(ok.any())
def run(n, clauses, path):
    with open(path,'w') as f:
        f.write("p cnf %d %d\n"%(n,len(clauses)))
        for cl in clauses: f.write(" ".join(map(str,cl))+" 0\n")
    out=subprocess.run(["./sat",path],capture_output=True,text=True).stdout
    if "UNSATISFIABLE" in out: return False,None
    if "SATISFIABLE" in out:
        model=[int(x) for x in out.split("\nv")[1].split("\n")[0].split()][:-1]
        return True,model
    raise Exception("bad output "+out)
bad=0; tot=0; nsat=0
path=tempfile.mktemp(suffix=".cnf")
for t in range(int(sys.argv[2]) if len(sys.argv)>2 else 400):
    n=random.randint(6,17); k=random.choice([2,3,3,3,4]); ratio={2:1.0,3:4.26,4:9.9}[k]*random.uniform(0.8,1.25)
    m=max(1,int(n*ratio)); clauses=[]
    for _ in range(m):
        vs=random.sample(range(1,n+1),min(k,n)); clauses.append([v if random.random()<0.5 else -v for v in vs])
    # sometimes add a few unit or binary clauses
    if random.random()<0.3: clauses.append([random.choice([-1,1])*random.randint(1,n)])
    truth=brute(n,clauses); got,model=run(n,clauses,path); tot+=1; nsat+=truth
    if got!=truth: bad+=1; print("MISMATCH n=%d m=%d truth=%s got=%s"%(n,len(clauses),truth,got)); continue
    if got:
        ms=set(model)
        if not all(any(l in ms for l in cl) for cl in clauses): bad+=1; print("BAD MODEL n=%d"%n)
print("tests",tot,"satisfiable",nsat,"failures",bad)
