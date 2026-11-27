import Main
open Lean Elab Command

#print RiemannHypothesis
#check @riemannHypothesis
#check (riemannHypothesis : _root_.RiemannHypothesis)
#print axioms riemannHypothesis

-- in which module is each name in the statement declared?
#eval show CommandElabM Unit from do
  let env ← getEnv
  for n in [``RiemannHypothesis, ``riemannZeta, ``DirichletCharacter.LFunction, ``RiemannZetaZerosOnLine,
            ``DirichletLZerosOnLine, ``riemannHypothesis] do
    match env.getModuleIdxFor? n with
    | some i => logInfo m!"{n}: declared in module {env.header.moduleNames[i]!}"
    | none => logInfo m!"{n}: declared in this file"
