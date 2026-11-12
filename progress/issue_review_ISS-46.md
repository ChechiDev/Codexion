# Issue review — ISS-46

**Verdict:** ISSUE_APPROVED

## Analysis
- Coverage / size / scope: ISS-46.1 preserves an expired deadline in `set_compile_start`; ISS-46.2 makes `run_monitor` report burnout before a simultaneously met target. Together they address both defects in the issue header. Two atomic subtasks, each with one declared orchestration source file, fit one issue.
- Dependencies / acceptance: ISS-46.1's phase dependencies and ISS-46.2's dependency on completed ISS-46.1 and 8.2.1 are satisfied; there is no cycle. Each criterion specifies observable C API or binary behavior and points to its subtask's single acceptance file. The zero-required binary case remains silent because `run_simulation` exits before starting the monitor.
- Migrations: `tests/acceptance/test_7.1.1.c`, `tests/acceptance/test_8.2.1.c`, and `tests/acceptance/test_9.2.3.c` exist. The specs identify the exact old-contract cases and the new expectations. In particular, ISS-46.2 names the target-before-burnout cases in 8.2.1 AC5/AC6 and 9.2.3 AC2; the remaining cases keep their meaning. No other sealed oracle found requires migration.
- Harness: the supplied first outside-sandbox `make check` exited 2. Its FAIL lines were:
  - `[FAIL]  ISS-46.1 is done but issue ISS-46 is not ISSUE_APPROVED (progress/issue_review_ISS-46.md)`
  - `[FAIL]  ISS-46.2 is in_progress but issue ISS-46 is not ISSUE_APPROVED (progress/issue_review_ISS-46.md)`
  - `[FAIL]  The harness is NOT healthy. Resolve the above before proceeding.`
  Compilation, tests, sync, gates, and sealed-oracle checks passed. The supplied counterfactual `make check` with `HARNESS_ASSUME_APPROVED=issue:ISS-46` exited 0 and reported `Harness invariants OK`; it assumes only this issue's verdict and matching seal. This satisfies the reviewer contract's issue re-gate exception. No tests were run in this sandbox.

## Per subtask
- ISS-46.1: ok — one-file deadline-preservation change, observable acceptance, and the required 7.1.1 oracle migration; completed dependency for ISS-46.2.
- ISS-46.2: ok — one-file scan-order change, six observable criteria, and both affected sealed monitor oracles declared for migration.
