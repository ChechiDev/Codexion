# Plan review — subtask 3.4.4

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `print_burnout` belongs in the log adapter; it uses the existing clock, line builder, stop state, mutexes and stdout API. The core formatter stays free of platform calls. The plan respects the subject's exact `burned out` line and serialized output requirement.
- Scope: one public function and three declared production files (`coders/src/log/print_burnout.c`, `coders/main.h`, `coders/Makefile`). Dependency 3.4.3 is closed and phase 03 is approved. The terminal stop protocol is specific to this function; monitor detection and the end-to-end 10 ms deadline remain assigned to later phases.
- Test strategy: `tests/acceptance/test_3.4.4.c` contains one public-API test per criterion. It exercises output, stop state, timestamp, ordering, lock and write errors, concurrent callers, build integration and Norm. The tests are intentionally red until the new symbol is implemented. The supplied harness check exited 0 before the oracle was written. Tests were not run in this sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_line_stop_and_unlocks` (red)
- AC2 → `test_ac2_unbuffered_pipe_visibility` (red)
- AC3 → `test_ac3_timestamp_bounds` (red)
- AC4 → `test_ac4_already_stopped` (red)
- AC5 → `test_ac5_reverse_stop_protocol` (red)
- AC6 → `test_ac6_no_states_after_burnout` (red)
- AC7 → `test_ac7_single_burnout_race` (red)
- AC8 → `test_ac8_burnout_serializes_against_states` (red)
- AC9 → `test_ac9_rejected_coder_commits_stop` (red)
- AC10 → `test_ac10_lock_failure_postconditions` (red)
- AC11 → `test_ac11_write_error` (red)
- AC12 → `test_ac12_build_symbol_and_norm` (red)
