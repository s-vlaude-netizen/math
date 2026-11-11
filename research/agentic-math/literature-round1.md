# Literature round 1: agentic LLMs for mathematical research

First pass, 2026-11-11. This is a map, not a survey.

**Method.** Eight keyword queries against the arXiv API (the exact strings are in
`tools/fetch_arxiv.py`), limited to 2025 and later, newest first. About 100 hits
were scanned by title, 25 abstracts were read, and the sections on weaknesses and
limitations were read in full text for the papers marked [R]. One striking claim
from a web search was checked against arXiv abstracts. The field moved faster than
my background knowledge, so everything below is "what the papers say", not what I
knew.

**Reading status.** [R] relevant sections read in full text; [A] abstract only;
[T] title only. A claim marked [A] may be qualified in the body.

## 1. Capability landmarks

* **Unit distance conjecture disproved (May 2026).** [A] An OpenAI model produced a
  counterexample to the Erdős unit distance conjecture; Alon, Bloom, Gowers et al.
  published a short human-verified version the same day (2605.20695); Sawin then
  gave an explicit exponent, more than n^1.014 unit distances (2605.20579).
* **Sum-product over the reals.** [A] 2607.20525 reports that a simple three-stage
  agent (proof plan, proof construction, review) produced correct disproofs of the
  Erdős–Szemerédi sum-product conjecture over R in 7 of 8 independent trials, and in
  the eighth reported an unresolved gap. The paper says a human disproof had just
  appeared. Neither claim is checked by me.
* **Aletheia, "Towards Autonomous Mathematics Research"** (Feng et al., 2602.10177). [R]
  An agent that generates, verifies and revises natural-language solutions.
* **Erdős case study** (Feng et al., 2601.22401). [R] The same system screened 700
  problems marked "Open" on the Erdős site.
* **Station** (2608.23691). [A + sections] Agents from different model families
  share a growing body of papers without a central coordinator. On 12 AlphaEvolve
  construction problems, 5 produced results the authors call novel, all backed by
  exact constructions or Lean-verified proofs. On autoconvolution problems it did
  worse than evolutionary search.
* **Bolzano** (2610.09769). [R partial] Parallel prover agents plus a verifier and a
  human-readable research record; about 200 of ~3,800 extracted open problems
  reported solved.
* **ProofCouncil** (2607.09474) [A]: 6 of 10 FirstProof problems judged correct up to
  minor revision. **Rethlas + Archon** (2604.03789) [A]: an open problem in
  commutative algebra resolved and Lean-verified "with essentially no human
  involvement".

## 2. Verification and fidelity

* **Verification abundance, adjudication scarcity** (2608.28997). [A] Argues for
  separate layers: derivational validity (a kernel checks it) and representational
  fidelity (does the formalization mean what it claims). Its example: ten results
  each shipped with a Lean certificate, and four weeks later one was still disputed
  over what its formalization meant. I did not see the third layer in the abstract.
* **Verification Autonomy Levels, L0–L5** (2608.19009). [A] Classifies a verifier by
  where its specification comes from. Key claim, the "completeness blind spot":
  checking that a candidate holds is possible, proving that no candidate was missed
  is not, outside formally specifiable properties.
* **Failure modes on research-level maths** (2606.24902). [A] Four modes: citation
  fabrication, premise smuggling, silent problem reformulation, local-to-global
  gaps. Small audit: 8 one-shot proofs from one weak model; none had a confirmed
  fabricated citation, all 8 contained a load-bearing claim asserted without
  justification. A premise-audit flagged with 5/5 precision and 50% proof-level
  recall.
* **Mechanical checks** (2610.20573) [A]: Lean formalizations checked with numerical
  and vacuity tests in addition to an LLM judge. **AIProver** (2610.05367) [A]:
  certificate-driven formalization, notes that compiling does not imply meaning.
  Several October 2026 papers target statement fidelity (2610.18498, 2610.25018,
  2610.32919) [T].
* **Strict step-level verification** (2606.10799) [A]: judging each step with its
  own context, to avoid "context poisoning".

## 3. Benchmarks and evaluation

* **HorizonMath** (2603.15617). [R partial] 113 predominantly unsolved problems in
  eight domains, chosen for a *generator–verifier gap*: hard to discover, cheap to
  check. Stated limits: matching a 20-digit numerical reference is not a proof, and
  the LLM compliance checker is imperfect.
* **RadonBench** (2610.33259) [A]: compares classical and learning-guided search on
  one open combinatorial-geometry problem. **DuoDiscover** (2610.17184) [A]: the
  cost of evaluating progress is the bottleneck. **SqueezeBench** (2610.13461) [T].

## 4. Choosing problems

* **Erdős case study.** [R] The authors conclude the "Open" status came from
  obscurity rather than difficulty.
* **LANTERN** (2609.32264). [A] Ranked 50 million pairs among 10,000 OEIS
  sequences; 62 verified relations; 13 survived a content screen; 4 judged novel;
  under 8 hours end to end.
* **Can LLMs generate interesting research problems?** (2603.18813) [A]: 665
  differential-geometry problems, many unknown to experts.

## 5. Long-horizon infrastructure

* **Stellar Colosseum** (2609.15983) [A]: model-agnostic harness with a readiness
  gate before decomposing a proof plan and verifier findings routed back to the
  affected part. Bolzano keeps a persistent human-readable research state.

## 6. Position papers

* **When proofs are abundant, what is a paper for?** (2611.12943) [A]: a paper is a
  choice of problem plus a chain of reasoning exposed to criticism, not only a
  verified statement. **Automation without understanding** (2607.06377) [A]: the
  capacity to verify and interpret is infrastructure.

## 7. Numbers worth keeping (primary text)

| Finding | Source |
|---|---|
| 700 "Open" Erdős problems screened, 13 positive results; 9 of them turned out to be in the literature already (4 independently rediscovered, 5 found as existing solutions) | 2601.22401 §1.2 |
| The abstract of the same paper splits it 5 novel / 8 literature; §1.2 lists 4 autonomous. I did not resolve the difference; possibly versions | 2601.22401 |
| Earlier AI-assisted claims on Erdős 1026, 397, 333, 281 were found to overlap the literature after being announced | 2601.22401 §1.4 |
| "The most challenging step for human experts was not verification, but determining if the solutions already existed in the literature" | 2601.22401 §1.6 |
| Of 200 candidates marked correct or incorrect: 31.5% technically correct under some reading, 6.5% correct for the *intended* reading (older base model) | 2602.10177 §5.2 |
| The model "tends to misinterpret the question in a way that is easiest to answer" (specification gaming); hallucinates references even with search | 2602.10177 §5.3 |
| "Remaining bottleneck: correctness, intended scope, and novelty"; failures include proofs of weaker versions | 2610.09769 conclusion |

## 8. Gaps in this round

Not read: FirstProof itself (referenced by three papers), the Quanta and Kalai
write-ups, anything outside arXiv (zbMATH, MathOverflow, the Erdős-site comment
threads), and the full text of most papers above. Not checked: whether the ideas
in `open-problems.md` already exist. Round 2 should start there.
