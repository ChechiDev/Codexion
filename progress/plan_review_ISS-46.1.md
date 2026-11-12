# Plan review — subtask ISS-46.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers and scope: one existing coder state function changes. The inclusive deadline check belongs under the coder mutex; the existing caller propagates result 1 and releases its dongles. No new port, adapter, header, or caller change is needed.
- Test strategy: seven observable criteria have one C acceptance test each. AC2, AC3, and AC4 are red against the current unconditional overwrite; AC5 also exercises the caller path. Lock, clock, and unlock failure handling remains a code review check because those failures cannot be forced deterministically.
- Migration: only the sealed `test_7.1.1.c` oracle was changed. AC1, AC3, and AC4 now prefill a live deadline with 100; AC2 and AC5 retain their behavior and assertions.
- The supplied outside-sandbox `make check` exited 0 at session start. The acceptance tests were not executed in this sandbox; the new test source passed a warning-clean syntax check.

## Acceptance tests (traceability)
- AC1 → `test_ac1_store_live_deadline` (currently passing behavior)
- AC2 → `test_ac2_keep_expired_deadline` (red against unconditional overwrite)
- AC3 → `test_ac3_inclusive_boundary` (red against unconditional overwrite)
- AC4 → `test_ac4_monitor_sees_expired_deadline` (red against unconditional overwrite)
- AC5 → `test_ac5_caller_releases_without_compiling` (red against unconditional overwrite)
- AC6 → `test_ac6_unchanged_paths` (currently passing behavior)
- AC7 → `test_ac7_build_symbol_norm` (currently passing behavior)
