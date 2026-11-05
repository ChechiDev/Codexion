# Plan review — subtask 7.3.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `run_coder` stays in the coder orchestration module and composes the already approved dongle, state, time, and log APIs. It adds no core dependency on platform code.
- Scope: one public thread routine, with local helpers for its cycle and exit paths, is atomic. The three declared files are `coders/src/coder/run_coder.c`, `coders/main.h`, and `coders/Makefile`.
- Test strategy: `tests/acceptance/test_7.3.1.c` observes the public routine, logs, simulation state, and resource state. The pipe is drained by a concurrent reader; shared observations are made after joining or under a mutex. The test supplies the stop until the monitor exists. The implementation-specific error paths remain explicit code-review checks. The tests are red because `run_coder` is not implemented yet. The supplied outside-sandbox `make check` exited 0 before these tests were added; no suite was run here. A syntax-only compile of the oracle passed.

## Acceptance tests (traceability)
- AC1 → `test_ac1_cycle_and_order` (red)
- AC2 → `test_ac2_shared_pair_and_state` (red)
- AC3 → `test_ac3_one_coder` (red)
- AC4 → `test_ac4_prompt_stop` (red)
- AC5 → `test_ac5_interrupted_compile` (red)
- AC6 → `test_ac6_already_stopped` (red)
- AC7 → `test_ac7_failure_exit_policy` (red)
- AC8 → `test_ac8_invalid_input` (red)
