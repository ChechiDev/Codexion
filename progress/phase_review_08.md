# Phase review — phase 08

**Verdict:** PHASE_APPROVED

## Analysis
- Coverage: task 8.1 supplies the per-coder deadline and global compile-target checks; task 8.2 combines them in a separate monitor routine, logs burnout within the subject's 10 ms budget, stops, and wakes dongle waiters. This delivers the phase goal without pulling in phase 09 lifecycle wiring.
- Scope: each pending subtask declares one public function in one new `coders/src/monitor/` file, plus the shared header and Makefile. Static helpers in 8.2.1 support that single routine. The shared files are claimed in the explicit 8.1.1 → 8.1.2 → 8.2.1 dependency chain.
- Dependencies and architecture: the chain is acyclic and follows the orchestration layer's use of existing state, time, log, and dongle APIs. No later phase is broken down.
- Acceptance: each subtask names its own `tests/acceptance/test_<id>.c` and observable public-API behavior. Boundary timing, mutex contention, target completion, wake-ups, invalid input, and output failure are covered. Source-specific implementation checks are kept in `Code review check:` rather than acceptance criteria. The timing oracle states its clock, capture method, and allowed test slack.

## Per subtask
- 8.1.1: ok — one deadline query; exact boundary, alternate duration, extreme starts, locking, and failure output are specified.
- 8.1.2: ok — one global target query; scan coverage, zero target, contention, and invalid state are specified.
- 8.2.1: ok — one monitor thread routine; target precedence, burnout reporting, stop behavior, waiter wake-up, one-coder behavior, and write failure are specified.
