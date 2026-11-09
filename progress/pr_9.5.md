## Overview

Task 9.5 connects the finished lifecycle driver to the command-line program. Valid arguments now start a complete simulation, normal outcomes return status 0, invalid arguments retain status 1, and internal run failures produce a fixed stderr diagnostic and status 2. The README explains these outcomes for users.

## Approach

The entry point keeps ownership of a stack-allocated `t_sim` and delegates setup, thread coordination, and cleanup to `run_simulation`. It performs no simulation work until argument validation succeeds. The lifecycle layer reports success or failure; the CLI layer owns the user-facing runtime message, and `main` maps the result to the process status. This preserves the lifecycle driver's contract for failures where a worker may still reference simulation storage.

## Decisions

The approved plan keeps the simulation structure in `main`'s stack frame so it remains valid until process exit on unrecoverable thread paths. The runtime error stays a fixed CLI message rather than exposing internal failure details. The existing unit test for valid arguments was updated to expect simulation output, since valid input now runs the program.

## Issues resolved

**Phase breakdown scope and ordering.** The initial phase 09 review requested that a combined coder result-handling change be split and that dependencies order shared header and build-file edits. The phase plan was revised to ten atomic subtasks with explicit dependencies, and the phase gate then approved it. These planning changes are recorded in `progress/review_history/phase-09.md` and the phase plan; they do not change task 9.5's implementation scope.

**Valid-input unit expectation.** Once `main` began running simulations, `tests/unit/test_main_usage.c` failed because it still expected valid arguments to produce empty stdout. The implementation report records the failure and correction. The assertion now requires output on the valid-input path, while invalid-input behavior remains covered separately.

## Issues open

Nothing open.

## Follow-ups

Task 9.5 completes the phase 09 entry point. The planned follow-up is phase 10's final integrated verification and documentation work.

## Branch notes


## Where to start reviewing

1. `docs/plan/phase-09-lifecycle/9.5.2-main-runs-simulation.md` — the approved entry-point contract and acceptance criteria.
2. `coders/main.c` — the validation-to-lifecycle handoff and process-status mapping.
3. `coders/src/cli/print_run_error.c` — the runtime diagnostic used by the entry point.
4. `README.md` — the user-facing description of process statuses.
