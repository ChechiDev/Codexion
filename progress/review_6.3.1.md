# Code review — subtask 6.3.1

**Verdict:** APPROVED

## Checkpoints
- C0: [x] `wake_dongles` uses only permitted pthread calls, has no globals or output, and the supplied check reports Norm clean.
- C1: [x] The supplied outside-sandbox `make check` exited 0; its log confirms base files, build, tests, and harness checks.
- C2: [x] One leaf is in progress, the plan and generated index are in sync, and `progress/plan_review_6.3.1.md` is `PLAN_APPROVED`.
- C3: [x] Dongle orchestration uses the existing `t_sim` context and single project header; no core dependency or new global state.
- C4: [x] One small public operation with a focused static helper; each lock, broadcast, and unlock failure is handled, and later dongles are still visited. The new source, header, and Makefile entry match the declared scope. No allocation, buffer, or external input is involved.
- C5: [x] The acceptance oracle covers AC1–AC6 through the public API and passed in the supplied check. The unit test checks repeat calls, unlocked mutexes, one and several dongles, and invalid input. The implementation report records clean Valgrind and ThreadSanitizer runs on the acceptance binary.
- C6: [ ] Close-stage documentation, status, history, and commit remain for the separate close gate.

## Findings
1. `coders/src/dongle/wake_dongles.c:19-26` locks each dongle before broadcast, attempts unlock even if broadcast fails, and reports any failed step.
2. `coders/src/dongle/wake_dongles.c:34-44` rejects null input before locking and continues through every dongle in index order after a per-dongle failure. It changes no simulation state.
3. `coders/main.h:185` exposes the prototype in the Dongle section; `coders/Makefile:26` includes the new source in the deliverable.
