# Review history: ISS-46

## Round 1: issue gate, ISSUE_APPROVED (2026-11-11 17:58)

### Issue review — ISS-46

**Verdict:** ISSUE_APPROVED

#### Analysis
- Coverage and size: two atomic subtasks address both ways a reached burnout deadline can be hidden; the work fits one issue and stays within its stated goal.
- Scope and architecture: each subtask declares one existing orchestration source file. The current `run_coder` path propagates a nonzero `set_compile_start` result and releases held dongles without marking a normal stop as a failure, so no caller edit is needed. The monitor change is confined to scan order.
- Dependencies and acceptance: all phase dependencies are done; ISS-46.2 follows ISS-46.1. Each subtask has observable acceptance criteria and one dedicated C oracle path. The synthetic joint-state monitor case tests the precedence rule; the program-boundary case retains silent success for a zero target.
- Migrations: `tests/acceptance/test_7.1.1.c` and `tests/acceptance/test_8.2.1.c` exist. The specs identify the exact old assertions that conflict with the new behavior and preserve the other sealed cases. No other sealed oracle requires migration based on the stated change.
- The supplied outside-sandbox `make check` exited 0; no tests were run in this review.

#### Per subtask
- ISS-46.1: ok — one state-update function, inclusive deadline rule, explicit 0/1/-1 contract, caller regression, and precise migration of the old prefill sentinel.
- ISS-46.2: ok — one scan-order change, burnout-first and target-only cases, and precise migration of the two old precedence cases.

## Round 2: issue gate, CHANGES_REQUESTED (2026-11-12 12:09)

### Issue review — ISS-46

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage, size, scope, and dependencies: the two atomic subtasks cover the expired-deadline overwrite and monitor precedence defects. Each declares one orchestration source file; ISS-46.2 depends on the completed ISS-46.1 and 8.2.1. The acceptance criteria are observable and each subtask names one C oracle path.
- Migrations: both listed files exist. ISS-46.2 now identifies the conflicting AC2 in `tests/acceptance/test_9.2.3.c`, along with AC5 and AC6 in `test_8.2.1.c`. The proposed burnout-first expectations match the issue goal, while the zero-required binary case remains covered separately.
- The supplied outside-sandbox `make check` exited 2. `tests/acceptance/test_9.2.3.c:174` still asserts `cap.len == 0` for an overdue, zero-required monitor state; the burnout-first implementation emits a burnout line. The check also reports that the changed ISS-46.2 spec has not passed its issue seal gate. The reviewer role forbids approval while the supplied harness check is red. No tests were run in this sandbox.

#### Per subtask
- ISS-46.1: ok — completed deadline-preservation change, with one declared production file and its sealed oracle migration.
- ISS-46.2: blocked by the red harness check; its revised migration scope and acceptance criteria are otherwise coherent.

#### Requested changes
1. Resolve the failing sealed `test_9.2.3.c` AC2 through the authorized migration and obtain a green outside-sandbox `make check`; rerun this issue gate with that result. The spec seal failure must also be resolved by the gate workflow.

## Round 3: issue gate, CHANGES_REQUESTED (2026-11-12 12:52)

### Issue review — ISS-46

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage, size, scope, dependencies and acceptance: the two subtasks cover the deadline overwrite and monitor precedence defects within one issue. Each changes one declared orchestration source file. ISS-46.2 depends on completed ISS-46.1 and 8.2.1; its six criteria describe observable behavior through one C acceptance path.
- Migrations: both listed sealed oracles exist. The ISS-46.2 spec identifies the conflicting AC5 and AC6 in `test_8.2.1.c` and AC2 in `test_9.2.3.c`, and limits their changes to the new burnout-first rule. No other sealed case was found that requires migration.
- The supplied outside-sandbox `make check` exited 2. Compilation and tests passed; its two failures say that ISS-46.1 is done and ISS-46.2 is in progress while the issue lacks an `ISSUE_APPROVED` verdict. The reviewer contract's hard rule forbids approval with a red harness check, even when this gate verdict is the cause of the check failures. No tests were run in this sandbox.

#### Per subtask
- ISS-46.1: ok — completed deadline preservation, one declared source file, and a specified sealed oracle migration.
- ISS-46.2: breakdown is sound; approval is blocked solely by the supplied red harness check.

#### Requested changes
1. Resolve the issue-gate check cycle in the gate workflow so this issue can be reviewed with an accepted green prerequisite, then rerun the issue gate. The breakdown itself needs no further change.

## Round 4: issue gate, ISSUE_APPROVED (2026-11-12 15:48)

### Issue review — ISS-46

**Verdict:** ISSUE_APPROVED

#### Analysis
- Coverage / size / scope: ISS-46.1 preserves an expired deadline in `set_compile_start`; ISS-46.2 makes `run_monitor` report burnout before a simultaneously met target. Together they address both defects in the issue header. Two atomic subtasks, each with one declared orchestration source file, fit one issue.
- Dependencies / acceptance: ISS-46.1's phase dependencies and ISS-46.2's dependency on completed ISS-46.1 and 8.2.1 are satisfied; there is no cycle. Each criterion specifies observable C API or binary behavior and points to its subtask's single acceptance file. The zero-required binary case remains silent because `run_simulation` exits before starting the monitor.
- Migrations: `tests/acceptance/test_7.1.1.c`, `tests/acceptance/test_8.2.1.c`, and `tests/acceptance/test_9.2.3.c` exist. The specs identify the exact old-contract cases and the new expectations. In particular, ISS-46.2 names the target-before-burnout cases in 8.2.1 AC5/AC6 and 9.2.3 AC2; the remaining cases keep their meaning. No other sealed oracle found requires migration.
- Harness: the supplied first outside-sandbox `make check` exited 2. Its FAIL lines were:
  - `[FAIL]  ISS-46.1 is done but issue ISS-46 is not ISSUE_APPROVED (progress/issue_review_ISS-46.md)`
  - `[FAIL]  ISS-46.2 is in_progress but issue ISS-46 is not ISSUE_APPROVED (progress/issue_review_ISS-46.md)`
  - `[FAIL]  The harness is NOT healthy. Resolve the above before proceeding.`
  Compilation, tests, sync, gates, and sealed-oracle checks passed. The supplied counterfactual `make check` with `HARNESS_ASSUME_APPROVED=issue:ISS-46` exited 0 and reported `Harness invariants OK`; it assumes only this issue's verdict and matching seal. This satisfies the reviewer contract's issue re-gate exception. No tests were run in this sandbox.

#### Per subtask
- ISS-46.1: ok — one-file deadline-preservation change, observable acceptance, and the required 7.1.1 oracle migration; completed dependency for ISS-46.2.
- ISS-46.2: ok — one-file scan-order change, six observable criteria, and both affected sealed monitor oracles declared for migration.
