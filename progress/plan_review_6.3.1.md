# Plan review — subtask 6.3.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `wake_dongles` belongs in dongle orchestration. It uses the existing simulation context and per-dongle condition variables; the plan adds no core dependency on pthreads or other adapters.
- Scope: one public function in `coders/src/dongle/wake_dongles.c`, with its declaration in `coders/main.h` and build entry in `coders/Makefile`. The three declared production files cover the change. Dependency 6.2.2 is done, and phase 06 is approved.
- Test strategy: the oracle uses the public `main.h` API and initialized simulation state. Mutex handshakes establish waiting threads before the stop broadcast in AC1, AC2 and AC5; AC3 exercises the immediate stop race over 200 fresh simulations. An alarm bounds hangs. AC4 checks preserved state and unlocked mutexes; AC6 checks null inputs. The tests are intentionally red until `wake_dongles` is declared and implemented. The supplied outside-sandbox `make check` exited 0 before this oracle was added; no tests were run in this review.
- Code review must inspect best-effort handling of pthread failures and lock ordering, which the specified acceptance scenarios cannot force deterministically.

## Acceptance tests (traceability)
- AC1 → `test_ac1_stop_wakes_all_dongles` (red)
- AC2 → `test_ac2_wakes_several_waiters` (red)
- AC3 → `test_ac3_no_lost_wakeup` (red)
- AC4 → `test_ac4_preserves_state_and_unlocks` (red)
- AC5 → `test_ac5_one_coder_wakes` (red)
- AC6 → `test_ac6_rejects_null_input` (red)
