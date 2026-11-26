# Which Claude session works on this repository

Updated 2026-11-26. Keep this file current whenever the working session changes.

**Rule: one active session per repository.** A message sent to an idle session wakes it
(in a fresh container) and it starts working, so two sessions on one repository means duplicate
work and, later, merge conflicts. Sessions never wake each other; only a message from the user
or a scheduled task does. The `/list-agents` command only lists sessions inside the same
container, so it shows nothing about other sessions.

| | Active (canonical) | Idle |
|---|---|---|
| Session | `session_012fG6WaCYEpznhJh1aAQAFu` "Integer sequences and Erdos research" | `session_01LR1qhmGpwbV1QMCqkT4dsy` "Offene Mathematikprobleme lösen" |
| Repositories | `math`, `quantumalgorithms` | `math` only |
| Branch | `ccr-dc987561-gkuljz` (both repositories) | `claude/open-math-problems-47rwoa` |
| State | working | idle since 2026-11-24 06:34 UTC; was woken by a message at 05:42 that day |

**What the idle session did** (all under `erdos1016/`, merged into the canonical branch on 2026-11-26,
merge commit `b3617f4`): b-file for A105206 ends with the blank line the OEIS asks for; a CP-SAT
cross-check of the 7-chord search (unusable: 12 of 12 attempts hit the 150 s limit); a symmetry-closure
test of the 7-chord solutions (weak); the 7-chord solver equals a naive enumeration at n = 9 (887,832
chord sets) and the 6-chord solver at n = 9..11. Its comparison at n = 10 was still running when its
container ended, so `erdos1016/arcform/xcheck/cmp_ref_k7_n10.log` is empty and **that comparison is not done**.
The k = 7 row of A105206 (N(7) = 114) is still not reproduced by a second search program.

**Switching sessions:** ask the active session to commit and push, then in the new session run
`git fetch` and merge the other session's branch before starting, and update the table above.
Archiving the idle session (it can be unarchived) prevents it from being woken by accident.
