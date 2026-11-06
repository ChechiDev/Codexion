# Review history: ISS-32.1

## Round 1: plan gate, PLAN_APPROVED (2026-11-06 11:38)

### Plan review — subtask ISS-32.1

**Verdict:** PLAN_APPROVED

#### Plan analysis
- Layers: `sleep_ms` remains in the time adapter; the return-contract change requires no new port or dependency. The existing `run_coder` calls at `coders/src/coder/run_coder.c:36`, `:51`, `:55`, and `:60` already leave on either nonzero result, so changing the caller is outside this subtask.
- Scope: the plan changes one function and declares two production files, `coders/src/time/sleep_ms.c` and the optional `coders/main.h`. It defers failure-state behavior to phase 09.
- Test strategy: the new oracle uses the public API to check return values, timing, concurrent stop, and coder behavior. The clock-failure path cannot be forced through ordinary public inputs and is explicitly reserved for code review. AC6 checks the deliverable build, symbol, and Norm. The tests are expected to be red because `sleep_ms.c:39` still returns -1 on an observed stop.
- Migration: only the AC3 and AC4 stop-result assertions in `tests/acceptance/test_3.3.1.c` changed from -1 to 1. Its AC1, AC2, and AC5 remain unchanged.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_nonpositive_duration` (suite red)
- AC2 → `test_ac2_running_duration` (suite red)
- AC3 → `test_ac3_already_stopped` (red)
- AC4 → `test_ac4_concurrent_stop` (red)
- AC5 → `test_ac5_coder_behavior` (suite red)
- AC6 → `test_ac6_build_symbol_and_norm` (suite red)

`tests/acceptance/test_ISS-32.1.c` compiled with `cc -Wall -Wextra -Werror -pthread -Icoders -fsyntax-only`; `git diff --check` passed. No tests or test-running make targets were run in the sandbox.

## Round 1: code gate, PLAN_APPROVED (2026-11-06 11:56)

### Plan review — subtask ISS-32.1

**Verdict:** PLAN_APPROVED

#### Plan analysis
- Layers: `sleep_ms` remains in the time adapter; the return-contract change requires no new port or dependency. The existing `run_coder` calls at `coders/src/coder/run_coder.c:36`, `:51`, `:55`, and `:60` already leave on either nonzero result, so changing the caller is outside this subtask.
- Scope: the plan changes one function and declares two production files, `coders/src/time/sleep_ms.c` and the optional `coders/main.h`. It defers failure-state behavior to phase 09.
- Test strategy: the new oracle uses the public API to check return values, timing, concurrent stop, and coder behavior. The clock-failure path cannot be forced through ordinary public inputs and is explicitly reserved for code review. AC6 checks the deliverable build, symbol, and Norm. The tests are expected to be red because `sleep_ms.c:39` still returns -1 on an observed stop.
- Migration: only the AC3 and AC4 stop-result assertions in `tests/acceptance/test_3.3.1.c` changed from -1 to 1. Its AC1, AC2, and AC5 remain unchanged.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_nonpositive_duration` (suite red)
- AC2 → `test_ac2_running_duration` (suite red)
- AC3 → `test_ac3_already_stopped` (red)
- AC4 → `test_ac4_concurrent_stop` (red)
- AC5 → `test_ac5_coder_behavior` (suite red)
- AC6 → `test_ac6_build_symbol_and_norm` (suite red)

`tests/acceptance/test_ISS-32.1.c` compiled with `cc -Wall -Wextra -Werror -pthread -Icoders -fsyntax-only`; `git diff --check` passed. No tests or test-running make targets were run in the sandbox.
