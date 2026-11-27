import Main
open Lean Elab Command

-- one environment per process: print, for each theorem and definition, the exact type (as the Lean kernel Expr), whether a proof term is present, and whether it mentions sorry.
#eval show CommandElabM Unit from do
  let env ← getEnv
  for n in [`riemannHypothesis, `riemannZeta_eq_zero_re, `LFunction_ne_zero_of_half_lt_re, `LFunction_eq_zero_re, `LFunction_eq_zero_re_of_isPrimitive] do
    match env.find? n with
    | some c =>
      let hasVal := (c.value? (allowOpaque := true)).isSome
      let hasSorry := ((c.value? (allowOpaque := true)).map (·.hasSorry)).getD false
      logInfo m!"THM {n} :: isTheorem = {c.isTheorem} :: type = {repr c.type} :: hasValue = {hasVal} :: valueMentionsSorry = {hasSorry}"
    | none => logInfo m!"THM {n} MISSING"
  for n in [`RiemannZetaZerosOnLine, `DirichletLNonvanishingRightOfHalf, `DirichletLZerosOnLine, `PrimitiveDirichletLZerosOnLine, `RiemannHypothesis] do
    match env.find? n with
    | some c => logInfo m!"DEF {n} :: type = {repr c.type} :: valueHash = {(c.value?.map (·.hash)).getD 0}"
    | none => logInfo m!"DEF {n} MISSING"
