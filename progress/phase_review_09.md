# Phase review — phase 09

**Verdict:** PHASE_APPROVED

## Analysis
- Coverage: tasks 9.1–9.5 deliver failure tracking, worker and monitor error reporting, thread startup and joining, whole-run cleanup, and the validated entry point. The one-coder and zero-required cases are explicit. Phase 10 retains only its title and goal.
- Scope: each of the ten subtasks has one primary function or state contract and declares one to three files. The previous combined coder change is split between 9.2.1 and 9.2.2. Living header, Makefile, and README updates are declared where required.
- Dependencies: no cycle; 9.1.2 precedes 9.3.1, 9.2.1 precedes 9.2.2, 9.3.1 and the worker-reporting changes precede 9.3.2, and 9.4.1 precedes 9.5.1. All shared production-file edits are ordered.
- Acceptance: every subtask names one `tests/acceptance/test_<id>.c` path and observable return, state, output, or thread-completion criteria. Failure paths that cannot be induced portably are assigned to explicit code-review checks. The supplied outside-sandbox `make check` exited 0; no tests were run here.

## Per subtask
- 9.1.1: ok; the failure and monitor fields, initialisation, and existing init contract are covered.
- 9.1.2: ok; `set_failed` is one synchronized state operation with success, failure, and concurrency checks.
- 9.2.1: ok; the acquisition wrapper preserves the print result, including the observable `/dev/full` failure.
- 9.2.2: ok; the coder routine distinguishes normal stop from failure and checks cleanup through its public API.
- 9.2.3: ok; the monitor distinguishes target met, burnout, and runtime error, including waiter wake-up.
- 9.3.1: ok; the join helper covers full, partial, monitor-only, and invalid counts; unforceable join failures are checked by source review.
- 9.3.2: ok; startup order, clock origin, one-coder run, and best-effort failed creation recovery are specified. The opportunistic creation-failure oracle reports when the path was not exercised.
- 9.4.1: ok; the driver specifies zero-target fast return, normal ends, runtime failure, and cleanup boundaries.
- 9.5.1: ok; the CLI error printer has exact stderr output criteria.
- 9.5.2: ok; the binary covers normal, burnout, one-coder, zero-target, runtime-error, argument-error, and leak outcomes.
