# How much compute for better AI, how much for doing math with the AI we have?

Short answer: it is an optimal-control / optimal-stopping question (operations research),
not philosophy, and the answer depends on **who** is spending.

## Model 1 - one group, capability is bought by someone else

Frontier labs spend 10^8..10^10 dollars per model. A math group's marginal compute changes
the next model by ~0, and it receives every improvement for free. Capability A(t) is then
exogenous, so the group's problem has no "build AI" arm; its optimum is f = 0 for model
training. What *is* worth spending on: verification (Lean, exact certificates, independent
second programs), scaffolding that makes existing models reliable, and problem selection.
Those raise the value of each unit of model compute and do not depend on the lab race.

## Model 2 - when to spend at all (optimal stopping)

A problem has value V(t) = V0 * exp(-d t) (d = how fast the result stops mattering:
priority, being scooped, a person's patience) and compute cost C(t) = C0 * exp(-r t)
(r = how fast the cost of a given capability falls: roughly 3-10x/year from hardware plus
algorithms).  Net payoff of acting at time t is V0 e^{-dt} - C0 e^{-rt}.
  * If r > d the payoff is increasing in t while it is negative or small: **wait**, the problem
    gets cheaper faster than it loses value.  Act when V0 e^{-dt*} = (r/d) C0 e^{-rt*}.
  * If d >= r act now.
So "100% math now" is right only for problems that are time-sensitive or already cheap; for
the expensive ones waiting is the optimum, and the right investment today is the cheap
preparation (formalisations, search reformulations, certificates), which keeps its value.

## Model 3 - a planner who controls all compute (global allocation)

Let B be the compute budget per period, f the fraction spent on capability. Capability
obeys A' = g(f B) with g concave (scaling laws: loss ~ compute^-alpha, alpha ~ 0.05..0.3),
and math output per period is m(A) * (1-f) B with m increasing.  With horizon T and no
discounting the Hamiltonian is linear in f for fixed A, so the optimum is **bang-bang with
a switch time**: put f = 1 (all into capability) early, then f = 0 (all into use) at the end,
because capability multiplies every later period's output.  Concave g plus discounting and
depreciation of models (a model is replaced after ~1 year) smooths this to an interior
f* that falls over time, solving  g'(f B) * integral of future m'(A) (1-f) B  =  m(A) B.
If m grows slowly in A for a task class (many open problems are search-limited, not
insight-limited) the switch happens earlier.  For the real world the sign of the answer
("mostly capability now") rests on estimates of alpha and of m'(A), which are the
quantities worth measuring: e.g. how the success rate on a fixed benchmark of open-problem
extensions scales with model size and with test-time compute.

## Numerical illustration (toy parameters, `allocation_model.py`)

Planner model with dA/dt = c (fB)^alpha - delta A, output (1-f) B A^beta, discount rho, horizon T
(default alpha 0.5, delta 0.3, beta 1, rho 0.1, T 10 years, 10 yearly intervals, coordinate
descent). Optimal share f of compute spent on building capability, per year:

| case | f(t), year 1 .. 10 | value |
|---|---|--:|
| default | .50 .35 .30 .30 .25 .25 .25 .20 .10 .05 | 6.91 |
| never build (f = 0) | 0 | 2.47 |
| build for 5 years, then use | 1 x5, 0 x5 | 3.72 |
| strongly diminishing returns, alpha 0.2 | .25 .20 .15 .15 .15 .10 .10 .10 .05 .05 | 9.76 |
| nearly linear, alpha 0.9 | .75 .45 .40 .40 .40 .40 .40 .30 .10 .00 | 5.14 |
| no depreciation | 1.0 .80 .45 .30 .20 .15 .10 .05 .05 0 | 17.54 |
| output barely depends on capability, beta 0.3 | .10 throughout, then 0 | 5.79 |
| impatient, rho 0.5 | .20 .20 .20 .15 .15 .15 .15 .15 .10 .05 | 1.90 |
| long horizon T = 30 (first 10 years shown) | .40 .30 .25 .30 .25 .30 .25 .25 .25 .10 | 10.84 |

Reading: with diminishing returns the optimum is **interior, not bang-bang**, and declines
over time; it never pays to put 0 into capability in the first years (f = 0 is worth 36% of the
optimum here), nor all of it forever. The share is highest when returns are nearly linear
(alpha 0.9) or models do not depreciate, and lowest when the output hardly depends on
capability (beta 0.3) or the planner is impatient. These are illustrations of the mechanism,
not estimates of the real alpha, beta, delta, which are what would have to be measured.

## Where it belongs

The optimisation itself is control theory / economics (a subfield of applied mathematics).
Neural nets are mathematical objects and learning theory is mathematics, but "how should
society allocate compute" depends on empirical parameters, so it is applied mathematics
with measured inputs, not pure mathematics.

## Practical rule used in this repository

Spend model compute only on problems where (a) the answer is exactly checkable by an
independent program or proof, and (b) the next value is not obviously hours away at 10x
cheaper later.  Everything else: prepare, and wait.
