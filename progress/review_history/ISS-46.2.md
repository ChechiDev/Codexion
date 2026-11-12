# Review history: ISS-46.2

## Round 1: plan gate, PLAN_APPROVED (2026-11-12 11:34)

### Plan review — subtask ISS-46.2

**Verdict:** PLAN_APPROVED

#### Plan analysis
- Layers and scope: the change is confined to the existing orchestration function `scan_step` in one declared production file. It preserves the existing burnout selection helper, logging, stop, failure, and wake-up paths. Dependencies ISS-46.1 and 8.2.1 are done, and the issue gate is `ISSUE_APPROVED`.
- Test strategy: the new C oracle observes `run_monitor` through the public `main.h` API and the deliverable binary. The joint-state cases distinguish burnout-first from the present target-first order. The supplied outside-sandbox `make check` exited 0 before these oracle edits; no tests were run in this sandbox. AC1 is expected to be red against the current `scan_step`, which checks `get_target_met` before `handle_burnout`.
- Migration: only the listed sealed oracle `tests/acceptance/test_8.2.1.c` was changed. Its AC5 and AC6 assertions, names, and comments now express burnout-first precedence; all other cases remain unchanged. The historical 8.2.1 spec remains sealed.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_burnout_wins_joint_observation` (red: current monitor exits on target).
- AC2 → `test_ac2_target_alone` (regression).
- AC3 → `test_ac3_burnout_alone` (regression).
- AC4 → `test_ac4_external_stop` (regression).
- AC5 → `test_ac5_zero_required_binary` (regression at program boundary).
- AC6 → `test_ac6_build_symbol_norm` (build and Norm gate).

## Round 2: plan gate, PLAN_APPROVED (2026-11-12 15:55)

### Plan review — subtask ISS-46.2

**Verdict:** PLAN_APPROVED

#### Plan analysis
- Layers and scope: `scan_step` is the single orchestration unit in the one declared production file. The stopped, burnout, and target order fits the issue goal and binding burnout rule. ISS-46.1 and 8.2.1 are done; the issue gate is `ISSUE_APPROVED`.
- Test strategy: the new C oracle observes `run_monitor` through `main.h` and the deliverable binary. AC1 is red against the current target-first `scan_step`; the remaining cases protect target-only, burnout-only, external-stop, zero-required binary, and build behavior. The supplied outside-sandbox `make check` exited 0 before these oracle edits. No tests were run in this sandbox; syntax compilation and `git diff --check` passed.
- Migration: only the two declared sealed oracles changed. The target-first cases in `test_8.2.1.c` AC5/AC6 and `test_9.2.3.c` AC2 now expect one burnout line. Other cases remain unchanged. The historical specs remain sealed.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_burnout_wins_joint_observation` (red against current monitor order).
- AC2 → `test_ac2_target_alone` (regression).
- AC3 → `test_ac3_burnout_alone` (regression).
- AC4 → `test_ac4_external_stop` (regression).
- AC5 → `test_ac5_zero_required_binary` (regression at the program boundary).
- AC6 → `test_ac6_build_symbol_norm` (build, symbol, and Norm gate).

## Round 1: code gate, CHANGES_REQUESTED (2026-11-12 16:39)

### Code review — subtask ISS-46.2

**Verdict:** CHANGES_REQUESTED

#### Checkpoints
- C0: [x] The burnout-first scan satisfies the binding deadline rule; the supplied harness log reports Norm and tests green.
- C1: [x] Required harness files and plan sync are present; outside-sandbox `make check` exited 0.
- C2: [x] ISS-46.2 is the sole active leaf, its plan is approved, and the generated index is in sync.
- C3: [x] The change stays in monitor orchestration, uses the existing state and helpers, and adds no dependency or global.
- C4: [ ] The one declared production file is in scope and its logic is focused, but its edited 42 header was not refreshed as required by the spec and conventions (`coders/src/monitor/run_monitor.c:9`). The harness reports a warning-clean build and passing tests; no new allocation, input, or secret handling is involved.
- C5: [x] AC1–AC6 have corresponding acceptance checks; the two declared sealed oracles were migrated. The existing unit test exercises monitor stop paths. The implementation report records TSan and Valgrind success for the new oracle; the migrated 8.2.1 oracle has a baseline-reproduced Valgrind timing assertion failure.
- C6: [ ] Close artifacts and commit are pending this code gate; no close review is made here.

#### Findings
1. `coders/src/monitor/run_monitor.c:9`: the `Updated:` timestamp still reads `2026/11/08 12:00:00`, although this file was edited for ISS-46.2. The spec's code review check explicitly requires refreshing that line with Stdheader; `docs/conventions.md` requires the same for later edits to legacy files. `progress/impl_ISS-46.2.md:2` confirms it was left untouched.

#### Requested changes
1. Refresh the 42 header `Updated:` line in `coders/src/monitor/run_monitor.c` using the official Stdheader plugin, then supply a fresh outside-sandbox green `make check` result for the code gate.

## Round 2: code gate, APPROVED (2026-11-12 17:14)

### Code review — subtask ISS-46.2

**Verdict:** APPROVED

#### Checkpoints
- C0: [x] Burnout is checked before the compile target, preserving the binding deadline and logging rules; the supplied harness log reports Norm green.
- C1: [x] Required harness files and plan sync are present; the supplied outside-sandbox `make check` exited 0.
- C2: [x] ISS-46.2 is the sole active leaf, its plan is approved, and the generated index is in sync.
- C3: [x] The change remains in monitor orchestration and reuses existing state and helpers; no new dependency or global is introduced.
- C4: [x] Only the declared production file changed. `scan_step` has one responsibility and no duplicated scan logic. The 42 `Updated:` line is refreshed (`coders/src/monitor/run_monitor.c:9`); the supplied check reports a warning-clean build and passing tests. Error paths and wake-ups remain intact.
- C5: [x] AC1–AC6 have corresponding public API or binary acceptance checks, and the two declared sealed oracles were migrated. The existing unit test covers target and burnout paths. The implementation report records TSan and Valgrind success for the new oracle; the migrated 8.2.1 oracle has a baseline-reproduced Valgrind timing assertion failure.
- C6: [ ] Close documentation, status update, history entry, and commit remain for the close gate.

#### Findings
- No blocking findings. `coders/src/monitor/run_monitor.c:69` checks external stop first, `:71` checks burnout, `:74` checks the target, and `:78` sleeps. The existing error and wake-up handling at `:83`–`:103` is unchanged.
