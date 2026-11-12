# Code review — subtask ISS-46.2

**Verdict:** APPROVED

## Checkpoints
- C0: [x] Burnout is checked before the compile target, preserving the binding deadline and logging rules; the supplied harness log reports Norm green.
- C1: [x] Required harness files and plan sync are present; the supplied outside-sandbox `make check` exited 0.
- C2: [x] ISS-46.2 is the sole active leaf, its plan is approved, and the generated index is in sync.
- C3: [x] The change remains in monitor orchestration and reuses existing state and helpers; no new dependency or global is introduced.
- C4: [x] Only the declared production file changed. `scan_step` has one responsibility and no duplicated scan logic. The 42 `Updated:` line is refreshed (`coders/src/monitor/run_monitor.c:9`); the supplied check reports a warning-clean build and passing tests. Error paths and wake-ups remain intact.
- C5: [x] AC1–AC6 have corresponding public API or binary acceptance checks, and the two declared sealed oracles were migrated. The existing unit test covers target and burnout paths. The implementation report records TSan and Valgrind success for the new oracle; the migrated 8.2.1 oracle has a baseline-reproduced Valgrind timing assertion failure.
- C6: [ ] Close documentation, status update, history entry, and commit remain for the close gate.

## Findings
- No blocking findings. `coders/src/monitor/run_monitor.c:69` checks external stop first, `:71` checks burnout, `:74` checks the target, and `:78` sleeps. The existing error and wake-up handling at `:83`–`:103` is unchanged.
