# A separate read of `src/Statements.lean` (the statements the proof repository defines itself)

Done 2026-11-28 on `grh-lean` commit `72fead10`, folder `grh-compact/` (the file is identical in `grh/`). The headline theorem
`riemannHypothesis` is stated with Mathlib's own `RiemannHypothesis` (see the README). The other four theorems are stated with
four definitions that the authors wrote in `src/Statements.lean`: 16 lines, `import Mathlib` and nothing else, no `open`, no
notation, no instances, no attributes. `Challenge.lean` (the five theorems with `sorry`) imports only that file, and `Main` reaches it through `New.Main`
(whose first line is `import Statements`), so both sides use the same constants; the dumps of the five theorem types and of the
definitions' values agree (`results/dump_Challenge.txt`, `results/dump_Main.txt`).

## The four definitions, read line by line

```lean
def RiemannZetaZerosOnLine : Prop :=
  ∀ s : ℂ, riemannZeta s = 0 → (¬∃ n : ℕ, s = -2 * ((n : ℂ) + 1)) → s ≠ 1 → s.re = 1 / 2

def DirichletLNonvanishingRightOfHalf : Prop :=
  ∀ (N : ℕ) [NeZero N] (χ : DirichletCharacter ℂ N) (s : ℂ),
    1 / 2 < s.re → (χ ≠ 1 ∨ s ≠ 1) → DirichletCharacter.LFunction χ s ≠ 0

def DirichletLZerosOnLine : Prop :=
  ∀ (N : ℕ) [NeZero N] (χ : DirichletCharacter ℂ N) (s : ℂ),
    0 < s.re → s.re < 1 → DirichletCharacter.LFunction χ s = 0 → s.re = 1 / 2

def PrimitiveDirichletLZerosOnLine : Prop :=
  ∀ (N : ℕ) [NeZero N] (χ : DirichletCharacter ℂ N), χ.IsPrimitive →
    ∀ s : ℂ, 0 < s.re → s.re < 1 → DirichletCharacter.LFunction χ s = 0 → s.re = 1 / 2
```

| Name | In words | Standard counterpart |
|---|---|---|
| `RiemannZetaZerosOnLine` | every zero of ζ other than -2, -4, -6, ... and other than 1 has real part 1/2 | the Riemann hypothesis; **the same term as Mathlib's `RiemannHypothesis`** (checked below) |
| `PrimitiveDirichletLZerosOnLine` | for every modulus N >= 1 and every primitive character χ mod N, every zero of L(s, χ) in the open strip 0 < Re s < 1 has real part 1/2 | the generalized Riemann hypothesis for Dirichlet L-functions, in its usual form |
| `DirichletLZerosOnLine` | the same for every character mod N, primitive or not | GRH including imprimitive characters (their extra Euler-factor zeros lie on Re s = 0, outside the open strip) |
| `DirichletLNonvanishingRightOfHalf` | L(s, χ) is not 0 for Re s > 1/2, except at the pole s = 1 of the principal character | a consequence of GRH, together with the known non-vanishing for Re s >= 1 |

Points a reader should check, and what I found:

* **Numerals.** `1 / 2`, `0`, `1` next to `s.re` could have been natural-number division (`1 / 2 = 0`). They are real numbers: printed with
  `pp.numericTypes`, the definitions read `s.re = (1 / 2 : ℝ)`, `(0 : ℝ) < s.re`, `s.re < (1 : ℝ)` (`results/audit_statements.txt`).
* **Moduli.** `[NeZero N]` means N >= 1 (Mathlib needs it to define `LFunction`); there is no modulus-0 case to worry about.
* **The strip is open.** Zeros with Re s <= 0 (the trivial zeros) and Re s = 1 are not constrained by the last three statements; the authors
  say so in their README, and that is the textbook reading of "non-trivial zero" for primitive characters.
* **The pole.** Only the principal character has a pole, at s = 1, where Mathlib assigns an arbitrary value. The nonvanishing statement
  excludes exactly that point (`χ ≠ 1 ∨ s ≠ 1`); the two zero statements never reach it (Re s < 1).
* **`DirichletCharacter.LFunction` is Mathlib's.** Its docstring: "the unique meromorphic function ℂ → ℂ which agrees with
  ∑' n, χ n / n ^ s wherever the latter is convergent". Mathlib proves `LFunction_eq_LSeries` (equal to the series for Re s > 1), differentiability
  everywhere except s = 1 for the principal character, `LFunction_modOne_eq` (modulus 1 is ζ), `LFunction_changeLevel` (the Euler
  factors of an imprimitive character) and non-vanishing for Re s >= 1. Holomorphy plus the series on Re s > 1 pins the function down.
* **Scope.** "GRH for Dirichlet L-functions" only: nothing about Dedekind zeta functions of number fields or other L-functions.

## Machine-checked cross-checks (`scripts/AuditStatements.lean`, output in `results/audit_statements.txt`)

These use only Mathlib and the four definitions, not the authors' proofs. The file compiles with 0 errors and 0 warnings, no `sorry`, and every
theorem depends only on `propext`, `Classical.choice`, `Quot.sound`.

| Lean theorem | Meaning |
|---|---|
| `audit_zeta_statement_is_mathlibs` | `RiemannZetaZerosOnLine ↔ RiemannHypothesis`, proved by `Iff.rfl` |
| `audit_all_of_primitive` | `PrimitiveDirichletLZerosOnLine → DirichletLZerosOnLine`: write χ as the primitive character of its conductor changed to level N; the extra Euler factors `1 - χ*(p) p^(-s)` have `‖χ*(p) p^(-s)‖ = ‖χ*(p)‖ p^(-Re s) < 1` for Re s > 0, so they never vanish in the strip |
| `audit_primitive_of_all` | the converse (immediate), so the two are equivalent |
| `audit_nonvanishing_of_all` | `DirichletLZerosOnLine → DirichletLNonvanishingRightOfHalf`, using Mathlib's non-vanishing for Re s >= 1 |
| `audit_rh_of_primitive` | `PrimitiveDirichletLZerosOnLine → RiemannHypothesis`: modulus 1 is ζ; zeros with Re s >= 1 are excluded by Mathlib; zeros with Re s <= 0 are exactly the trivial ones, by Mathlib's functional equation `riemannZeta_one_sub` |
| `audit_cluster` | the three consequences together |

So **all five theorems of the repository are consequences of one statement, `PrimitiveDirichletLZerosOnLine`**, using Mathlib's own theorems, and the
strongest of the four authors' definitions (`DirichletLZerosOnLine`) is equivalent to it. Whoever accepts that one statement as "GRH for Dirichlet
L-functions" accepts all five, and none of the authors' statements asks for more than the textbook one. In the other direction the checks also show that the
statements are not weaker than they look: modulus 1 alone already gives Mathlib's `RiemannHypothesis`, and `IsPrimitive` is not degenerate (otherwise the
all-characters statement could not follow from the primitive one).

## What is left to trust

* That Mathlib's `DirichletCharacter.LFunction` and `IsPrimitive` are what their names say (documented and tied to the series, as above; the same
  standing as Mathlib's `riemannZeta`), and that the reader agrees `PrimitiveDirichletLZerosOnLine` is the generalized Riemann hypothesis for Dirichlet
  L-functions.
* The Lean kernel and Mathlib, as always. This read says nothing about the proof itself.

## Reproduce

After the build described in `README.md`, from `grh-compact/`: `lake env lean /path/to/scripts/AuditStatements.lean` (about 10 to 30 seconds).
