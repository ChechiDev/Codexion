# Plan review — subtask 9.2.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `run_coder` is orchestration over existing state, logging, timing and dongle APIs. The spec adds no core dependency on I/O or platform code.
- Scope: one routine in one declared production file, `coders/src/coder/run_coder.c`. Its exit handling and private helpers serve that routine; no public identifier or extra production file is required.
- Test strategy: the oracle exercises only the public API in `coders/main.h`. It observes normal stop, resource release, output, and failure state. Count overflow, exhausted ticket and `/dev/full` deterministically exercise three real error paths. Each case has a watchdog; pipe output is drained concurrently. The supplied pre-session `make check` exited 0. The acceptance tests were not run in this sandbox and are expected to be red against the current routine, whose failure exit does not call `set_failed`.
- A syntax-only compile of `tests/acceptance/test_9.2.2.c` passed with `-std=c11 -Wall -Wextra -Werror -pthread`.

## Acceptance tests (traceability)
- AC1 → `test_ac1_normal_stop`
- AC2 → `test_ac2_stop_during_compile`
- AC3 → `test_ac3_stop_during_rest`
- AC4 → `test_ac4_zero_length_phases`
- AC5 → `test_ac5_single_coder`
- AC6 → `test_ac6_already_stopped`
- AC7 → `test_ac7_real_failures` (red against the current routine)
- AC8 → `test_ac8_invalid_input`
