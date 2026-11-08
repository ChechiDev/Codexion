# Plan review — subtask 9.3.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: thread creation and recovery belong in lifecycle orchestration; the spec reuses the time, state, wake, worker, monitor and join APIs without moving platform behavior into core code. The clock write is ordered before thread creation so existing worker reads have a defined origin.
- Scope: one public function, `start_threads`, with its source, header declaration and deliverable Makefile entry declared as the three affected files. Dependencies 9.3.1, 9.2.2 and 9.2.3 are done. NULL, clock, monitor-creation, coder-creation and recovery-join outcomes are specified separately. Failure paths that cannot be induced reliably are assigned to the code review check.
- Test strategy: `tests/acceptance/test_9.3.2.c` exercises the public API and observable output/state. It drains stdout concurrently and joins before reading completed state or freeing storage. The resource-limit attempt runs in a child and distinguishes exercised, cleanly not exercised, and real failure. Sanitizer and Valgrind runs skip that attempt with a diagnostic. The supplied outside-sandbox baseline `make check` exited 0; no tests were run in this sandbox. The new oracle is intentionally red until `start_threads` is implemented and declared.

## Acceptance tests (traceability)
- AC1 → `test_ac1_normal_start_reaches_target` (red)
- AC2 → `test_ac2_single_coder_burns_out` (red)
- AC3 → `test_ac3_creation_failure_recovers` (red)
- AC4 → `test_ac4_null_simulation_rejected` (red)
