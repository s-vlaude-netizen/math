# sat.c — a small CDCL SAT solver

Written because no solver was installed (and none could be installed) while working on
Erdős problem 160. Two watched literals with blockers, first-UIP learning with local clause
minimisation, VSIDS by linear scan (fine for a few hundred variables), phase saving, Luby
restarts, LBD-based deletion of learnt clauses. DIMACS input, prints `s SATISFIABLE` and a
`v` line, or `s UNSATISFIABLE`.

    gcc -O2 -march=native -o sat sat.c -lm
    ./sat file.cnf [conflict_limit]

**Validation** (`test_sat.py`, `test_sat_big.py`): random 2-, 3- and 4-SAT near the threshold
compared with a brute-force truth table, SAT models re-checked clause by clause.
450 instances (n = 6..22 variables; 306 satisfiable), 0 disagreements. On the colouring
problem it reproduces the known extremal values for 3, 4 and 5 colours.

**Not good**: about 14x slower than the specialised search in `erdos160/color4ap.c` on the
5-colour proof (25 s against 1.7 s), and the UNSAT answers have no proof certificate. Treat an
UNSAT from this solver as checked only where it was also reproduced by something else.
