# Plan review — subtask 9.2.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers / scope: The change belongs in the coder orchestration wrapper. The spec declares one production file and one result-handling responsibility; dependencies 7.2.1 and ISS-34.1 are complete.
- Test strategy: AC1–AC4 preserve successful pair and single-dongle acquisition, preexisting stop, and real write failure behavior. AC5 uses the test binary's `write` hook to make the simulation stop at the failed write, then checks the public `take_coder_dongles` result, untouched output sentinel, released dongles, and unlocked mutexes. This distinguishes the required behavior from the current implementation, which re-reads the stopped flag and would return 0. The oracle is intentionally left red. The supplied outside-sandbox `make check` exited 0 before this oracle was added; no test was run here. A syntax-only C compile of the oracle passed.

## Acceptance tests (traceability)
- AC1 → `test_ac1_running_pair` (regression)
- AC2 → `test_ac2_stopped_before_take` (regression)
- AC3 → `test_ac3_real_print_error` (regression)
- AC4 → `test_ac4_single_coder` (regression)
- AC5 → `test_ac5_print_error_with_stop` (red)
