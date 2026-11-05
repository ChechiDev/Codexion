# Code review — subtask 8.2.1

**Verdict:** APPROVED

## Checkpoints
- C0: [x] Subject-compliant monitor cadence, serialized burnout log and stop, allowed calls, no globals; supplied check reports Norm green.
- C1: [x] Supplied outside-sandbox `make check` exited 0; base files, plan sync and harness integrity are green.
- C2: [x] 8.2.1 is the sole active leaf, its plan review is `PLAN_APPROVED`, and the generated index is in sync. `progress/current.md` identifies the active subtask.
- C3: [x] `coders/src/monitor/run_monitor.c` orchestrates existing monitor, state, log and dongle APIs; it introduces no reverse layer dependency or direct mutex ownership. The public prototype is in the project's single header.
- C4: [x] The supplied check covers clean compilation and tests. The one new source matches the declared scope; static helpers keep selection in one place. Error exits attempt stop and wake without holding a mutex; there is no allocation, unbounded copy, secret or external input path.
- C5: [x] `tests/acceptance/test_8.2.1.c` has one public-API test per AC1–AC11 and passed in the supplied check. `tests/unit/test_run_monitor.c` exercises NULL, target met, zero target and burnout paths. The implementation report records clean Valgrind and ThreadSanitizer runs.
- C6: [ ] Close-phase work remains: reference documentation, `done` status, history and single commit are intentionally pending this code gate.

## Findings
- No blocking findings. `coders/src/monitor/run_monitor.c:64-72` follows the specified scan order and checks `usleep`; lines 87-92 distinguish an external stop from exits that require wake-up.

## Requested changes
- None.
