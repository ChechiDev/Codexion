# Review history: ISS-34

## Round 1: issue gate, CHANGES_REQUESTED (2026-11-06 13:00)

### Issue review — ISS-34

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage, size, scope and dependencies: one subtask covers the issue goal, declares one adapter file, and depends only on completed subtasks. The caller checks at `coders/src/coder/run_coder.c:34,49,53` and `coders/src/coder/take_coder_dongles.c:26` treat both nonzero results alike.
- Acceptance and migration: AC1–AC7 use one issue oracle path. The four `migrates:` files exist, and their listed stop-result assertions and loop conditions are the sealed expectations that need changing. Rejected-input, mutex-failure and write-failure assertions must retain -1.
- Contract conflict: the header says invalid coder input remains -1 (`docs/plan/issues/ISS-34-refactor-print-state-returns-1-on-stop-1-only-on-error-3-4.md:38-40`), and the subtask says every invalid coder is a failure (`docs/plan/issues/ISS-34-refactor-print-state-returns-1-on-stop-1-only-on-error-3-4/ISS-34.1-print-stop-result.md:36-43`). But `print_state` checks only the state before reading the stop flag (`coders/src/log/print_state.c:46-56`); coder validation occurs later in `build_log_line` (`coders/src/log/build_log_line.c:28-33`). Replacing only the stopped branch's -1 with 1 makes `print_state(&stopped_sim, 0, STATE_COMPILING)` return 1. AC4 tests invalid coders only while running, and AC5 tests only an invalid state after a stop, so the oracle would miss this violation.

#### Per subtask
- ISS-34.1: contract and acceptance conflict with the proposed one-line change for invalid coder input after a stop.

#### Requested changes
1. Specify and test that an invalid coder remains -1 when the simulation is stopped, then adjust the approach and code-review check to validate the coder before the stopped return. Keep the declared production scope to `print_state.c` and preserve the existing caller behavior and the four listed oracle migrations.

## Round 2: issue gate, ISSUE_APPROVED (2026-11-06 13:09)

### Issue review — ISS-34

**Verdict:** ISSUE_APPROVED

#### Analysis
- Coverage and size: one atomic subtask changes the return value for a suppressed line while retaining -1 for failures. It covers the imported issue without adding caller changes.
- Scope and architecture: the spec declares only `coders/src/log/print_state.c`. Its new coder validation before the stop read resolves the previous rejected-input conflict within that file. The adapter keeps its mutex order and output path. The production callers at `coders/src/coder/run_coder.c:34,49,53` and `coders/src/coder/take_coder_dongles.c:26` already treat nonzero results alike.
- Dependencies: `3.4.3`, `3.4.6`, `7.2.1`, and `7.3.1` are done; there is no cycle.
- Acceptance: AC1–AC7 have observable outcomes and one required oracle path. AC5 now checks invalid coder IDs after stop, including the case missed in the prior review. Source-only details are reserved for the code review check.
- Migrations: all four listed sealed acceptance files exist. A search of `tests/acceptance/` found no other direct `print_state` expectations requiring migration. The spec identifies the exact post-stop assertions and loop conditions while preserving -1 for actual failures. The supplied outside-sandbox `make check` exited 0; no tests were run in this sandbox.

#### Per subtask
- ISS-34.1: ok.

## Round 3: issue gate, ISSUE_APPROVED (2026-11-06 13:36)

### Issue review — ISS-34

**Verdict:** ISSUE_APPROVED

#### Analysis
- Coverage and size: ISS-34.1 is one atomic change to `print_state`'s return contract. It covers the imported issue without changing caller behavior or adding unrelated work.
- Scope and architecture: the sole declared production file is `coders/src/log/print_state.c`, in the logging adapter. The spec keeps the existing lock order and output path, and requires invalid coder input to be rejected before the stop read.
- Dependencies: 3.4.3, 3.4.6, 7.2.1, and 7.3.1 are done. There is no cycle.
- Acceptance: AC1–AC7 specify observable outcomes through `tests/acceptance/test_ISS-34.1.c`. The revised AC6(b) checks that an already stopped caller returns without taking dongles or printing; it correctly omits the heap-empty assertion, because the completed 6.3.4 contract leaves requests queued on stop. Source-only requirements remain in the code review check.
- Migrations: the four listed sealed oracle files exist, and the spec limits changes to assertions and loop conditions that expect -1 after stop. Other failure expectations remain -1. No additional sealed oracle is identified as needing migration.
- Harness state: the supplied outside-sandbox `make check` exited 2 because the edited ISS-34.1 oracle does not match its plan-gate seal. The subsequent ISS-34.1 plan gate must review and reseal that oracle. This issue verdict approves the revised breakdown; it does not resolve the oracle seal. No tests were run in this sandbox.

#### Per subtask
- ISS-34.1: ok.
