# Code review — subtask 7.3.1

**Verdict:** APPROVED

## Checkpoints
- C0: [x] `coders/src/coder/run_coder.c` uses the allowed APIs, follows the required take/compile/release/rest cycle, and passed Norm in the supplied check.
- C1: [x] Supplied outside-sandbox `make check` exited 0; its log confirms the base files, build, tests, and harness gates.
- C2: [x] `make status` shows one in-progress leaf, 7.3.1, with `PLAN_APPROVED`; the supplied check confirms plan sync and gate consistency.
- C3: [x] The coder routine stays in orchestration, uses the simulation context, introduces no global state or include cycle, and has one public thread entry point with local helpers.
- C4: [x] The supplied check covers warning-clean build, Norm, and tests. The three declared production files alone changed. The routine checks dependency failures, releases at most once per acquisition, and reuses existing state, dongle, sleep, and log functions. No allocation, unsafe buffer operation, or sensitive output is added.
- C5: [x] `tests/acceptance/test_7.3.1.c` has one public-API test for each AC1–AC8 and passed in the supplied check. `tests/unit/test_run_coder.c` covers invalid input, pre-stopped state, and compile-count failure. The implementer reports Valgrind and ThreadSanitizer clean on the acceptance binary; these were not rerun in this sandbox.
- C6: [x] Code-gate review only; reference documentation, done status, history, and the single commit belong to the later close gate.

## Findings
1. No blocking findings. In `coders/src/coder/run_coder.c:15-24`, the failure path attempts release only while ownership is recorded, then calls both stop and wake. In `coders/src/coder/run_coder.c:27-42` and `:58-68`, ownership is cleared before release, including the single-coder path. In `coders/src/coder/run_coder.c:70-95`, normal stop and zero-duration cycles terminate as specified.
