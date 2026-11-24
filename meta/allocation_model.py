"""Numerical solution of the planner model in compute-allocation.md.

State: capability A.  Control: fraction f of the compute budget B spent on building capability.
    dA/dt = c * (f*B)**alpha - delta*A            (diminishing returns, models depreciate)
    maths output rate = A**beta * (1-f) * B       (using what exists)
    maximise  integral_0^T exp(-rho t) * A**beta * (1-f) * B dt
f is piecewise constant on K intervals; coordinate descent on a grid.
"""
import math, sys

def value(fs, T=10.0, B=1.0, c=1.0, alpha=0.5, delta=0.3, beta=1.0, rho=0.1, A0=1.0, sub=20):
    K=len(fs); dt=T/(K*sub); A=A0; V=0.0; t=0.0
    for k in range(K):
        f=fs[k]
        for _ in range(sub):
            V+=math.exp(-rho*t)*(A**beta)*(1-f)*B*dt
            A+=dt*(c*(f*B)**alpha-delta*A); t+=dt
    return V

def optimise(K=10, grid=21, sweeps=30, **kw):
    fs=[0.5]*K
    best=value(fs,**kw)
    for _ in range(sweeps):
        improved=False
        for k in range(K):
            for j in range(grid):
                trial=fs[:]; trial[k]=j/(grid-1)
                v=value(trial,**kw)
                if v>best+1e-12: best=v; fs=trial; improved=True
        if not improved: break
    return fs,best

if __name__=="__main__":
    print("baseline: f=0 for all time  ->", round(value([0.0]*10),3))
    print("baseline: f=1 until T/2 then 0 ->", round(value([1.0]*5+[0.0]*5),3))
    for label,kw in [("diminishing returns alpha=0.5, depreciation 0.3 (default)",{}),
                     ("strongly diminishing alpha=0.2",{"alpha":0.2}),
                     ("nearly linear alpha=0.9",{"alpha":0.9}),
                     ("no depreciation delta=0",{"delta":0.0}),
                     ("output barely depends on capability beta=0.3",{"beta":0.3}),
                     ("impatient rho=0.5",{"rho":0.5}),
                     ("long horizon T=30",{"T":30.0})]:
        fs,v=optimise(**kw)
        print(f"{label:62s} value {v:8.2f}  f(t) by interval: "+" ".join(f"{x:.2f}" for x in fs))
