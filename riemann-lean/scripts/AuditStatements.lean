import Statements

/-!
# Audit of `src/Statements.lean` (the proof repository's own statement definitions)

Everything here uses only Mathlib and the four definitions of `src/Statements.lean`, never the authors' proofs.

* (A) prints the four definitions with the numeral types visible;
* (B) shows that `RiemannZetaZerosOnLine` is Mathlib's `RiemannHypothesis` (by `Iff.rfl`);
* (C) proves that the textbook statement `PrimitiveDirichletLZerosOnLine` (GRH for primitive characters) implies the other
  four, using only Mathlib's L-function API; conversely `DirichletLZerosOnLine` implies it trivially.
  So accepting that one statement as "GRH for Dirichlet L-functions" is accepting all five theorems of the repository.
-/

open DirichletCharacter

-- (A) the definitions as Lean elaborated them
set_option pp.numericTypes true in
#print RiemannHypothesis
set_option pp.numericTypes true in
#print RiemannZetaZerosOnLine
set_option pp.numericTypes true in
#print DirichletLNonvanishingRightOfHalf
set_option pp.numericTypes true in
#print DirichletLZerosOnLine
set_option pp.numericTypes true in
#print PrimitiveDirichletLZerosOnLine

-- (B) the zeta statement is Mathlib's
theorem audit_zeta_statement_is_mathlibs : RiemannZetaZerosOnLine ↔ RiemannHypothesis := Iff.rfl

-- (C1) all characters follow from primitive characters (Euler factors of an imprimitive character vanish only on Re s = 0)
theorem audit_all_of_primitive (h : PrimitiveDirichletLZerosOnLine) : DirichletLZerosOnLine := by
  intro N _ χ s hs0 hs1 hz
  have hne : s ≠ 1 := fun h1 => by rw [h1] at hs1; simp at hs1
  have : NeZero χ.conductor := ⟨χ.conductor_ne_zero⟩
  have hχ := χ.changeLevel_primitiveCharacter
  rw [← hχ, LFunction_changeLevel χ.conductor_dvd_level χ.primitiveCharacter (Or.inr hne)] at hz
  rcases mul_eq_zero.mp hz with hz | hz
  · exact h _ χ.primitiveCharacter χ.primitiveCharacter_isPrimitive s hs0 hs1 hz
  · exfalso
    obtain ⟨p, hp, hp0⟩ := Finset.prod_eq_zero_iff.mp hz
    have hp2 : 2 ≤ p := (Nat.prime_of_mem_primeFactors hp).two_le
    have h1 : χ.primitiveCharacter (p : ZMod χ.conductor) * (p : ℂ) ^ (-s) = 1 :=
      (sub_eq_zero.mp hp0).symm
    have hnorm := congrArg norm h1
    rw [norm_mul, norm_one, Complex.norm_natCast_cpow_of_pos (by omega)] at hnorm
    have hle : ‖χ.primitiveCharacter (p : ZMod χ.conductor)‖ ≤ 1 := DirichletCharacter.norm_le_one _ _
    have hlt : ((p : ℕ) : ℝ) ^ ((-s).re) < 1 := by
      apply Real.rpow_lt_one_of_one_lt_of_neg
      · exact_mod_cast hp2
      · simpa using hs0
    have hpos : 0 < ((p : ℕ) : ℝ) ^ ((-s).re) := Real.rpow_pos_of_pos (by positivity) _
    have := mul_le_mul_of_nonneg_right hle hpos.le
    linarith

-- (C2) the primitive statement is a special case of the all-characters one
theorem audit_primitive_of_all (h : DirichletLZerosOnLine) : PrimitiveDirichletLZerosOnLine :=
  fun N _ χ _ s hs0 hs1 hz => h N χ s hs0 hs1 hz

-- (C3) nonvanishing to the right of 1/2 follows from the zeros-on-the-line statement plus Mathlib's nonvanishing for Re s >= 1
theorem audit_nonvanishing_of_all (h : DirichletLZerosOnLine) : DirichletLNonvanishingRightOfHalf := by
  intro N _ χ s hs hχs hz
  by_cases h1 : 1 ≤ s.re
  · exact DirichletCharacter.LFunction_ne_zero_of_one_le_re χ hχs h1 hz
  · have := h N χ s (by linarith) (not_le.mp h1) hz
    linarith

-- (C4) Riemann's hypothesis (Mathlib's statement) follows from the primitive statement at modulus 1
theorem audit_rh_of_primitive (h : PrimitiveDirichletLZerosOnLine) : RiemannHypothesis := by
  intro s hs hnt hs1
  have h1 : (1 : DirichletCharacter ℂ 1).IsPrimitive := isPrimitive_one_level_one
  have hL : ∀ t : ℂ, DirichletCharacter.LFunction (1 : DirichletCharacter ℂ 1) t = riemannZeta t :=
    fun t => by simp [DirichletCharacter.LFunction_modOne_eq]
  by_cases hge : 1 ≤ s.re
  · exact absurd hs (riemannZeta_ne_zero_of_one_le_re hge)
  by_cases hpos : 0 < s.re
  · exact h 1 1 h1 s hpos (not_le.mp hge) ((hL s).trans hs)
  -- Re s <= 0: only the trivial zeros are possible (functional equation)
  exfalso
  push Not at hpos
  by_cases hs0 : s = 0
  · subst hs0
    rw [riemannZeta_zero] at hs
    norm_num at hs
  set t : ℂ := 1 - s with ht
  have ht_re : 1 ≤ t.re := by simp only [ht, Complex.sub_re, Complex.one_re]; linarith
  have hst : s = 1 - t := by simp [ht]
  have ht1 : t ≠ 1 := by
    intro h1'
    apply hs0
    rw [hst, h1']
    simp
  have htn : ∀ n : ℕ, t ≠ -n := by
    intro n hn
    have := congrArg Complex.re hn
    simp only [Complex.neg_re, Complex.natCast_re] at this
    have : (0 : ℝ) ≤ n := Nat.cast_nonneg n
    linarith
  have hfe := riemannZeta_one_sub htn ht1
  have e : 1 - t = s := by simp [ht]
  rw [e, hs] at hfe
  have hz : riemannZeta t ≠ 0 := riemannZeta_ne_zero_of_one_le_re ht_re
  have hG : Complex.Gamma t ≠ 0 := Complex.Gamma_ne_zero htn
  have hpi : (Real.pi : ℂ) ≠ 0 := by exact_mod_cast Real.pi_ne_zero
  have hpow : (2 * (Real.pi : ℂ)) ^ (-t) ≠ 0 := by
    rw [Complex.cpow_ne_zero_iff]
    left
    exact mul_ne_zero two_ne_zero hpi
  have hcos : Complex.cos (Real.pi * t / 2) = 0 := by
    by_contra hc
    exact (mul_ne_zero (mul_ne_zero (mul_ne_zero (mul_ne_zero two_ne_zero hpow) hG) hc) hz) hfe.symm
  obtain ⟨k, hk⟩ := Complex.cos_eq_zero_iff.mp hcos
  have htk : t = 2 * k + 1 := by
    have : (Real.pi : ℂ) * t = (Real.pi : ℂ) * (2 * k + 1) := by linear_combination 2 * hk
    exact mul_left_cancel₀ hpi this
  have hsk : s = -2 * (k : ℂ) := by rw [hst, htk]; ring
  have hre : s.re = -2 * (k : ℝ) := by rw [hsk]; simp
  have hk0 : (0 : ℤ) ≤ k := by
    have : (0 : ℝ) ≤ (k : ℝ) := by linarith
    exact_mod_cast this
  have hk1 : k ≠ 0 := by
    intro h0
    apply hs0
    rw [hsk, h0]
    simp
  obtain ⟨n, hn⟩ : ∃ n : ℕ, k = (n : ℤ) + 1 := ⟨(k - 1).toNat, by omega⟩
  exact hnt ⟨n, by rw [hsk, hn]; push_cast; ring⟩

-- (D) the whole cluster
theorem audit_cluster :
    (PrimitiveDirichletLZerosOnLine ↔ DirichletLZerosOnLine) ∧
    (PrimitiveDirichletLZerosOnLine → DirichletLNonvanishingRightOfHalf) ∧
    (PrimitiveDirichletLZerosOnLine → RiemannZetaZerosOnLine) :=
  ⟨⟨audit_all_of_primitive, audit_primitive_of_all⟩,
   fun h => audit_nonvanishing_of_all (audit_all_of_primitive h),
   fun h => audit_rh_of_primitive h⟩

#print axioms audit_zeta_statement_is_mathlibs
#print axioms audit_all_of_primitive
#print axioms audit_primitive_of_all
#print axioms audit_nonvanishing_of_all
#print axioms audit_rh_of_primitive
#print axioms audit_cluster
