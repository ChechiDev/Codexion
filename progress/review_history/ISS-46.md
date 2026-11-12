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
