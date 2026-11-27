# Machine check of the Lean proof of the Riemann hypothesis (compact edition)

Done on 2026-11-27 in a cloud container with 4 cores and 16 GB RAM. This folder holds the scripts and the
results only, not the authors' Lean source (see "Reproduce").

## What was checked

* **Source:** <https://github.com/lsernau/grh-lean>, commit `72fead102f663d5c7a407964a13dc0cf9d977207`, directory
  `grh-compact/`: 794 Lean files, 112,827 lines (about 1,900 printed pages at 60 lines per page).
  Lean v4.35.0 (commit `eb6cafb7`), Lake 5.0.0, Mathlib `56a005855ea17e0b0958f73ec801ae140b0a1679`
  (taken from the prebuilt Mathlib cache with `lake exe cache get`; Mathlib itself is 2.34 million lines).
* **Not built:** the 17,374,215-line original (175,187 files, Lean 4.33.1; 14-part tarball from alpo.ge, announced
  in <https://alpo.ge/remark.pdf>). I downloaded and scanned it (the counts match the published ones) but did not build it:
  the verifiers report about 8 h on a 192-core machine, which is roughly 770 thread-hours, and 82-99 GiB of RAM
  for the kernel check.

## Results

| Check | Result |
|---|---|
| `lake build` (clean environment, no credentials) | "Build completed successfully (9843 jobs)": all 794 project modules built, 0 errors, 298 warning lines (264 deprecations), none mentioning `sorry`. About 2 h wall clock on 4 cores; summed module time 7.6 h; slowest module 178 s. See `results/build_summary.txt`. |
| `#print axioms` for the five theorems | `[propext, Classical.choice, Quot.sound]` for all five (`results/axioms.txt`). |
| The headline statement | `riemannHypothesis : RiemannHypothesis`. `RiemannHypothesis` and `riemannZeta` are declared in `Mathlib.NumberTheory.LSeries.RiemannZeta`; `DirichletCharacter.LFunction` in `Mathlib.NumberTheory.LSeries.DirichletContinuation`. The project does not redefine them. The other four statements (GRH for Dirichlet L-functions) are the four definitions in the 16-line `src/Statements.lean`, which imports only Mathlib (`results/statement.txt`). |
| Challenge vs solution | `Challenge.lean` (the five statements with `sorry`) and `Main` give the same five theorem types, as Lean `Expr`s, and the same five definitions (type and value hash). The challenge proofs are `sorry`, the solution proofs contain none (`results/dump_Challenge.txt`, `results/dump_Main.txt`, compared with `diff`). This is a stand-in for the authors' tool Comparator, without its kernel replay. |
| Source scan of `src/` | No `sorry`, `axiom`, `native_decide`, `unsafe`, `implemented_by`, `extern`, `opaque`, `partial def`, `skipKernelTC`, `addDecl`, `addDeclWithoutChecking`, `import Lean`, custom syntax or macros, `IO` file or process calls. Three uses of `decide +kernel`, covered by the axiom check. |
| Kernel replay with `leanchecker` | **794 of 794 modules OK, 0 failures, all logs empty.** Each module is replayed on its own through Lean's kernel against its imports (`scripts/lc_one.sh`, 4 at a time by `scripts/lc_all.sh`); 26.6 min wall clock (`results/leanchecker_results.txt`). |

## What this does not show

* It is one more independent compile on another machine, not a review. The announcement material says that no
  human has read the proof through yet, and nothing here judges the mathematics. What was checked is that Lean's
  kernel accepts the five theorems as stated, and that the headline statement is Mathlib's formulation of the
  Riemann hypothesis.
* `leanchecker` replays through the same Lean kernel. Its own documentation calls it a tool to detect "environment
  hacking" and says it is not an external verifier, so a kernel bug would not be caught. Mathlib's own
  declarations came from the prebuilt cache and were not replayed here. The authors' README says they ran
  Comparator on this folder (its `comparator.json` enables the independent kernel nanoda) and that this needs about
  64 GB of memory and a few hours; that was not run here.
* The three Dirichlet-L theorems say nothing about zeros with real part at most 0 (authors' README).

## Reproduce

```bash
git clone https://github.com/lsernau/grh-lean && cd grh-lean && git checkout 72fead102f663d5c7a407964a13dc0cf9d977207
cd grh-compact
# install elan (https://github.com/leanprover/elan); it reads lean-toolchain and fetches Lean v4.35.0
lake exe cache get && lake build                    # about 2 h on 4 cores
lake env lean scripts/PrintAxioms.lean              # the five axiom lists
lake env lean /path/to/scripts/Statement.lean       # statement and where each name is declared
lake build Challenge                                # five `sorry` warnings are expected
lake env lean /path/to/scripts/DumpChallenge.lean   # then DumpMain.lean; compare the THM and DEF lines
lake env leanchecker Statements                     # one module; scripts/lc_all.sh loops over all of them
```

`scripts/run.sh` is the clean-environment wrapper used for every command (paths are those of this container).

## Lessons for reuse of the scripts

* Load one Mathlib-sized environment per process. Two in one `lean --run` process fell back to heap copies of the
  `.olean` files (9.6 GB heap) and thrashed the disk; the two `Dump*.lean` files do one side each.
* `ConstantInfo.value?` returns `none` for theorems unless `allowOpaque := true`.
* A `leanchecker` process shows 6-7 GB resident, but only about 250 MB of that is private heap; the rest is the
  shared `.olean` mapping, so four in parallel are fine on 16 GB.

## Sources

Wikipedia "Riemann hypothesis"; Lean community blog <https://leanprover-community.github.io/blog/posts/riemann-hypothesis/>;
Andrew Sutherland's verification pages <https://math.mit.edu/~drew/grh/> and <https://math.mit.edu/~drew/shortgrh/>;
Alpöge's announcement <https://alpo.ge/remark.pdf> (about 4 pages); <https://github.com/lsernau/grh-lean>.
