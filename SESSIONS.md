# Which Claude session works on this repository

Updated 2026-11-27. Keep this file current whenever the working session changes.

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

## Why background jobs die, and how to keep them alive (measured 2026-11-27)

Source: the container's own lifecycle logs, `/tmp/claude-code-*.diag.log` and `/tmp/environment-manager-*.diag.log`.

- While a turn is running, or a **registered** background task is running (a Bash call with `run_in_background`),
  the harness writes `session_keepalive_*` heartbeats and nothing is stopped. The 1.5 h Lean build on 11-27
  ran through 55 minutes of user silence this way.
- When a turn ends and no registered task is running (`bg_tasks: {}`), the worker receives SIGTERM about
  5 minutes later: 4 min 43 s after the turn ended on 11-27 09:14 (shutdown 09:19:26), exactly 5:00 for a stray
  wake-up at 11:08. Everything the worker started dies with it, including plain `nohup ... &` jobs. That is what
  ended the n = 13 Erdős-272 search and the Kelvin scans; the session had also stopped its own monitor loops at 09:14.
- The next user message boots a fresh container (uptime restarts, 20:45 UTC on 11-27) **on the same disk**:
  files survive, tracked or untracked (files from 11-22 are still there), processes do not.
- So a message from the user every few hours does not keep jobs alive. Run each long job as a registered
  background task (at most 2 h each; its completion notification wakes the session, which then starts the next),
  and give every long job a checkpoint and resume path anyway. Usage limits are separate: on 11-26 the session
  stopped at the 5-hour limit and sat idle for 24 h.
- Not known: whether a session has a hard maximum lifetime.
