# Review history: ISS-46.1

## Round 1: plan gate, PLAN_APPROVED (2026-11-11 18:10)

### Plan review — subtask ISS-46.1

**Verdict:** PLAN_APPROVED

#### Plan analysis
- Layers and scope: one existing coder state function changes. The inclusive deadline check belongs under the coder mutex; the existing caller propagates result 1 and releases its dongles. No new port, adapter, header, or caller change is needed.
- Test strategy: seven observable criteria have one C acceptance test each. AC2, AC3, and AC4 are red against the current unconditional overwrite; AC5 also exercises the caller path. Lock, clock, and unlock failure handling remains a code review check because those failures cannot be forced deterministically.
- Migration: only the sealed `test_7.1.1.c` oracle was changed. AC1, AC3, and AC4 now prefill a live deadline with 100; AC2 and AC5 retain their behavior and assertions.
- The supplied outside-sandbox `make check` exited 0 at session start. The acceptance tests were not executed in this sandbox; the new test source passed a warning-clean syntax check.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_store_live_deadline` (currently passing behavior)
- AC2 → `test_ac2_keep_expired_deadline` (red against unconditional overwrite)
- AC3 → `test_ac3_inclusive_boundary` (red against unconditional overwrite)
- AC4 → `test_ac4_monitor_sees_expired_deadline` (red against unconditional overwrite)
- AC5 → `test_ac5_caller_releases_without_compiling` (red against unconditional overwrite)
- AC6 → `test_ac6_unchanged_paths` (currently passing behavior)
- AC7 → `test_ac7_build_symbol_norm` (currently passing behavior)

## Round 1: code gate, CHANGES_REQUESTED (2026-11-11 18:41)

### Code review — subtask ISS-46.1

**Verdict:** CHANGES_REQUESTED

#### Checkpoints
- C0: [x] `set_compile_start` preserves an inclusive expired deadline under the coder mutex, matching the subject's burnout rule and `get_burned_out`.
- C1: [x] The supplied outside-sandbox `make check` exited 0; its log confirms the harness, build, tests, and plan sync.
- C2: [x] ISS-46.1 is the sole in-progress leaf, its plan gate is approved, and the generated index is in sync.
- C3: [x] The change stays in the coder orchestration function and adds no dependency or shared global state.
- C4: [ ] The new unit test has a timing-sensitive assertion with only a 1 ms live-deadline margin (finding 1). Production scope and error handling otherwise match the spec.
- C5: [ ] The acceptance oracle is present and green in the supplied check, but the unit regression test can fail under ordinary scheduling delay (finding 1). The implementation report records clean Valgrind and ThreadSanitizer runs of the acceptance binary.
- C6: [ ] Close documentation, status, history, and commit follow a successful code gate; this subtask is not closed.

#### Findings
1. `tests/unit/test_set_compile_start.c:61-62`: after setting elapsed time to approximately 1000 ms, the test changes `last_compile_start` to 201. With an 800 ms burnout interval, that deadline is 1001 ms. A delay of just 1 ms before the second call makes `set_compile_start` correctly return 1, while the test requires 0. This creates a flaky regression test on a normal scheduler. The acceptance test already uses 400 for a 200 ms margin (`tests/acceptance/test_ISS-46.1.c:164-168`).

#### Requested changes
1. Give the live-deadline branch in the unit test a practical time margin, while retaining the exact inclusive boundary assertion at 200. Update the unit test only, then supply a green outside-sandbox check for another code review.

## Round 2: code gate, APPROVED (2026-11-11 18:57)

### Code review — subtask ISS-46.1

**Verdict:** APPROVED

#### Checkpoints
- C0: [x] The inclusive burnout deadline is preserved, consistent with the subject and `get_burned_out`.
- C1: [x] The supplied outside-sandbox `make check` exited 0; the log confirms the build, tests, plan sync, and harness invariants.
- C2: [x] ISS-46.1 is the sole in-progress leaf, its plan gate is approved, and the generated index is synchronized.
- C3: [x] The change remains in the coder orchestration function, with no new dependency or global state.
- C4: [x] Only the declared production file changed. The function keeps the check and write under one mutex, handles errors, and contains no duplicated logic. The unit test now gives the live deadline a 200 ms margin while retaining the inclusive boundary case.
- C5: [x] All seven acceptance criteria have corresponding tests, and the supplied check is green. The unit test covers both return paths and mutex release. The implementation report records clean Valgrind and ThreadSanitizer runs of the acceptance binary.
- C6: [ ] Close documentation, status, history, and commit are reserved for the close gate; this subtask is still in progress.

#### Findings
- None. The prior unit-test timing finding is resolved at `tests/unit/test_set_compile_start.c:61-63`.
