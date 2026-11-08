# Review history: phase-09

## Round 1: phase gate, CHANGES_REQUESTED (2026-11-07 18:21)

### Phase review — phase 09

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 9.1–9.5 cover failure state, worker reporting, thread startup and joining, whole-run cleanup, and the entry point, including the one-coder and zero-target cases. Phase 10 remains unbroken down.
- Scope: each spec declares 1–3 production files and one acceptance test path, but 9.2.1 combines two independently reviewable function changes.
- Dependencies: the graph has no apparent cycle and follows the layers, but shared edits to `coders/main.h` and `coders/Makefile` are not fully ordered by explicit `depends` edges.
- Acceptance: criteria describe observable return values, output, state, and thread completion. Source-only properties are assigned to code review checks.

#### Per subtask
- 9.1.1: ok; the data fields and initialisation form one state contract.
- 9.1.2: ok; one state setter, with observable locking and state criteria.
- 9.2.1: split required; `take_coder_dongles` must preserve the `print_state` result, then `run_coder` must classify normal stop versus failure. These are separate function contracts in separate files (`9.2.1-coder-reports-failure.md:5-7`, `:23-30`).
- 9.2.2: ok; one monitor routine and its error exits.
- 9.3.1: missing explicit dependency on 9.1.2 for shared `coders/main.h` and `coders/Makefile` edits (`9.1.2-set-failed.md:4-8`, `9.3.1-join-threads.md:4-8`).
- 9.3.2: ok; depends on 9.3.1 and the worker-reporting subtasks, subject to the 9.2.1 split.
- 9.4.1: ok; depends on 9.3.2.
- 9.5.1: missing explicit ordering with 9.4.1 for shared `coders/main.h` and `coders/Makefile` edits (`9.4.1-run-simulation.md:4-8`, `9.5.1-print-run-error.md:4-8`).
- 9.5.2: ok; depends on both the run driver and error printer.

#### Requested changes
1. Split 9.2.1 into atomic subtasks: one changes `take_coder_dongles`, and a dependent one changes `run_coder`. Give each its own acceptance path and update the phase tasks, index, and downstream dependencies.
2. Add explicit `depends` links to order every pair of subtasks that edits the same production file. In particular, order 9.1.2 before 9.3.1 and 9.4.1 before 9.5.1, or choose another acyclic order that preserves the functional dependencies.

## Round 2: phase gate, PHASE_APPROVED (2026-11-07 18:42)

### Phase review — phase 09

**Verdict:** PHASE_APPROVED

#### Analysis
- Coverage: tasks 9.1–9.5 deliver failure tracking, worker and monitor error reporting, thread startup and joining, whole-run cleanup, and the validated entry point. The one-coder and zero-required cases are explicit. Phase 10 retains only its title and goal.
- Scope: each of the ten subtasks has one primary function or state contract and declares one to three files. The previous combined coder change is split between 9.2.1 and 9.2.2. Living header, Makefile, and README updates are declared where required.
- Dependencies: no cycle; 9.1.2 precedes 9.3.1, 9.2.1 precedes 9.2.2, 9.3.1 and the worker-reporting changes precede 9.3.2, and 9.4.1 precedes 9.5.1. All shared production-file edits are ordered.
- Acceptance: every subtask names one `tests/acceptance/test_<id>.c` path and observable return, state, output, or thread-completion criteria. Failure paths that cannot be induced portably are assigned to explicit code-review checks. The supplied outside-sandbox `make check` exited 0; no tests were run here.

#### Per subtask
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
