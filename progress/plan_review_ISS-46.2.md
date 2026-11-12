# Plan review — subtask ISS-46.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers and scope: `scan_step` is the single orchestration unit in the one declared production file. The stopped, burnout, and target order fits the issue goal and binding burnout rule. ISS-46.1 and 8.2.1 are done; the issue gate is `ISSUE_APPROVED`.
- Test strategy: the new C oracle observes `run_monitor` through `main.h` and the deliverable binary. AC1 is red against the current target-first `scan_step`; the remaining cases protect target-only, burnout-only, external-stop, zero-required binary, and build behavior. The supplied outside-sandbox `make check` exited 0 before these oracle edits. No tests were run in this sandbox; syntax compilation and `git diff --check` passed.
- Migration: only the two declared sealed oracles changed. The target-first cases in `test_8.2.1.c` AC5/AC6 and `test_9.2.3.c` AC2 now expect one burnout line. Other cases remain unchanged. The historical specs remain sealed.

## Acceptance tests (traceability)
- AC1 → `test_ac1_burnout_wins_joint_observation` (red against current monitor order).
- AC2 → `test_ac2_target_alone` (regression).
- AC3 → `test_ac3_burnout_alone` (regression).
- AC4 → `test_ac4_external_stop` (regression).
- AC5 → `test_ac5_zero_required_binary` (regression at the program boundary).
- AC6 → `test_ac6_build_symbol_norm` (build, symbol, and Norm gate).
