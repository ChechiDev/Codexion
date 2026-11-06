# Issue review — ISS-32

**Verdict:** ISSUE_APPROVED

## Analysis
- Coverage and size: ISS-32.1 is one atomic change to `sleep_ms`'s return contract. It covers the issue goal without adding phase 09's failure-state behavior.
- Scope and architecture: the adapter-layer change declares two production files. `run_coder.c` is consistently excluded; its existing nonzero-result checks preserve coder behavior. The optional `main.h` touch is limited to contract documentation.
- Dependencies: 3.3.1 and 7.3.1 are done; there are no issue-local dependencies or cycles.
- Acceptance: AC1–AC4 check the observable return and timing contract. AC5 runs coder threads against an initialized simulation, checks real compile progress and shutdown, and cannot pass vacuously. AC6 checks the deliverable build, symbol, and Norm. All six criteria name the single issue oracle path.
- Migration: `tests/acceptance/test_3.3.1.c` exists and only its stop-result assertions in AC3 and AC4 need to change from -1 to 1. The spec states the superseded behavior and leaves the sealed 3.3.1 spec intact. No other sealed oracle needs migration.
- The stated `is_stopped` mutex-failure limitation matches the sealed 3.2.1 contract. Clock failures remain a code-review check because ordinary inputs cannot force them.

## Per subtask
- ISS-32.1: ok — scope, dependencies, acceptance, and migration are consistent with the issue header and existing callers.
