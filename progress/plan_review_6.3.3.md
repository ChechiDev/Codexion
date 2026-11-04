# Plan review — subtask 6.3.3

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `release_dongles` is one orchestration function. It uses the time adapter and dongle synchronization through the public API, leaves the core heap and readiness policy unchanged, and introduces no global state or output.
- Scope: one function in `coders/src/dongle/release_dongles.c`, with its declaration in `coders/main.h` and build entry in `coders/Makefile`; all three production files are declared. Dependencies 6.2.1 and 6.3.2 are done and phase 06 is approved.
- Test strategy: six black-box acceptance tests exercise the public function and observable dongle state. Waiters use a handshake and a 60-second hang guard; AC6 holds the second mutex while a release thread owns the first, then checks that the release timestamp follows the wait. These tests are intentionally red until the function is implemented. The supplied outside-sandbox `make check` exited 0 before this oracle was written; no tests were run here.

## Acceptance tests (traceability)
- AC1 → `test_ac1_releases_pair_at_current_time` (red)
- AC2 → `test_ac2_wakes_both_waiters` (red)
- AC3 → `test_ac3_preserves_queue_and_cooldown` (red)
- AC4 → `test_ac4_releases_single_dongle` (red)
- AC5 → `test_ac5_rejects_invalid_input_unchanged` (red)
- AC6 → `test_ac6_release_time_follows_lock_wait` (red)
