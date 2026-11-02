# Plan review — subtask 5.2.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: one adapter cleanup function releases the heap, condition variable, and mutex owned by `init_dongle`; it adds no core-to-adapter dependency.
- Scope: one function and three declared production files (`destroy_dongle.c`, `main.h`, `Makefile`), consistent with the approved phase and the current state.
- Test strategy: `tests/acceptance/test_5.2.2.c` exercises only the public `main.h` API and observable dongle state. Each criterion has one test. The tests are intentionally red until the new prototype and implementation exist; none were run in this sandbox. The supplied outside-sandbox starting `make check` exited 0.
- Error handling: NULL is covered by AC5. Unportable pthread destroy failures are reserved for the stated code review check; teardown order and best-effort completion are also reviewed there.

## Acceptance tests (traceability)
- AC1 → `test_ac1_destroy_empty_dongle` (red)
- AC2 → `test_ac2_destroy_queued_dongle` (red)
- AC3 → `test_ac3_destroy_used_sync` (red)
- AC4 → `test_ac4_reinitialize_after_destroy` (red)
- AC5 → `test_ac5_null_dongle` (red)
