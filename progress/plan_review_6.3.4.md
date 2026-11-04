# Plan review — subtask 6.3.4

**Verdict:** PLAN_APPROVED

## Plan analysis

- Layers: two-dongle arbitration belongs in `coders/src/dongle/`; the plan reuses the existing request, readiness, heap, time, stop, and wake APIs. It keeps logging outside the dongle locks and follows the subject's FIFO/EDF, cooldown, and mutex requirements.
- Scope: one public function, `take_dongles`, with three declared production files (`take_dongles.c`, `main.h`, `Makefile`). The one-dongle case is reserved for 6.3.5.
- Test strategy: `tests/acceptance/test_6.3.4.c` has one public API test per acceptance criterion. Queue observations use dongle mutexes; completion order uses a test mutex. Timed polling and the process alarm are hang guards. Fatal syscall and pthread failure paths remain code review checks because they cannot be forced deterministically. This re-review moves AC6's final `owners[]` assertion after all worker joins, so concurrent workers cannot write the array during that read.
- Session baseline: the supplied outside-sandbox `make check` exited 0. No tests or test-running commands were run in this sandbox. The implementation already exists for this re-review, so the original red-oracle state cannot be inferred from this session.

## Acceptance tests (traceability)

- AC1 → `test_ac1_free_pair` (originally red)
- AC2 → `test_ac2_head_of_both` (originally red)
- AC3 → `test_ac3_fifo_and_edf_order` (originally red)
- AC4 → `test_ac4_cooldown` (originally red)
- AC5 → `test_ac5_stop_while_waiting` (originally red)
- AC6 → `test_ac6_concurrent_progress` (originally red; ownership check synchronized by joining all workers)
- AC7 → `test_ac7_invalid_input` (originally red)
