# Plan review — subtask 9.2.3

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `run_monitor` coordinates existing state, logging and dongle APIs in the orchestration layer. The failure flag remains synchronized through `set_failed`; no domain logic is placed in an adapter.
- Scope: one routine, one declared production file (`coders/src/monitor/run_monitor.c`), and no new public API. The specified four exits preserve the earlier scan order and assign unforceable dependency failures to code review.
- Test strategy: seven black-box C tests use `run_monitor` and the public simulation API. They observe stdout, stop/failure state, completion, lock release and waiter completion. The failed-write cases use `/dev/full` in children forked before test threads exist. The 8.2.1 acceptance oracle remains the latency and normal wake-up regression check. The supplied outside-sandbox `make check` exited 0 before this oracle was added; no tests were run here. The new file passed a syntax-only compile.

## Acceptance tests (traceability)
- AC1 → `test_ac1_target_met_clean` (expected green)
- AC2 → `test_ac2_zero_required_clean` (expected green)
- AC3 → `test_ac3_logged_burnout_clean` (expected green)
- AC4 → `test_ac4_burnout_write_failure` (red under current implementation)
- AC5 → `test_ac5_external_stop_untouched` (expected green)
- AC6 → `test_ac6_lock_state_after_exit` (expected green)
- AC7 → `test_ac7_error_wakes_waiters` (red under current implementation)

The code gate must inspect the specified error-exit calls, because a waiter can wake spuriously and this oracle cannot force failures of the other dependencies without fault injection.
