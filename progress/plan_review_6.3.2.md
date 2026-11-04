# Plan review — subtask 6.3.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `wait_for_dongles` is one orchestration function using the existing time adapter and dongle synchronization primitives. The pure readiness policy remains separate. Its caller owns the readiness and stop decisions.
- Scope: one function in `coders/src/dongle/wait_for_dongles.c`, with its declaration in `coders/main.h` and build entry in `coders/Makefile`; the three files are declared, and dependencies 6.2.2 and 6.3.1 are complete.
- Test strategy: four black-box checks use the public `main.h` API and initialized simulation state. They cover the bounded wait, release of the second mutex while waiting, repeated ownership restoration, and invalid inputs. The spec assigns the unobservable condition-variable choice and runtime-failure cleanup to code review. The new function and declaration do not exist yet, so this acceptance file is intentionally red. The supplied outside-sandbox `make check` exited 0 before this review; no tests were run here.

## Acceptance tests (traceability)
- AC1 → `test_ac1_timed_slice_restores_both_mutexes` (red)
- AC2 → `test_ac2_second_mutex_is_free_during_wait` (red)
- AC3 → `test_ac3_repeated_waits_preserve_ownership` (red; leak check at code gate)
- AC4 → `test_ac4_invalid_input_preserves_mutexes` (red)
