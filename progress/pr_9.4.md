## Overview

Task 9.4 adds the lifecycle driver that turns initialized simulation components into a complete run. It owns the setup, thread start, join, failure inspection, and cleanup sequence while leaving user-facing runtime error reporting to task 9.5.

## Approach

The driver composes the lifecycle APIs established by task 9.3 and the failure reporting contract from task 9.2. It returns immediately when zero compiles are required, otherwise initializes caller-owned state, starts the monitor and coder threads, and joins them before reading the shared failure flag. After successful joins it always attempts cleanup and reports an error if either the simulation or cleanup failed. The caller's storage remains valid on unrecoverable join paths where a thread may still refer to it.

## Decisions

Zero required compiles is treated as a successful no-op before simulation state is touched. Startup recovery failure and join failure both return the same `-1` result, so the caller treats all errors uniformly and ends the process without reusing the storage. Target completion and burnout are both normal outcomes.

## Issues resolved

None were recorded for task 9.4. Its subtask plan and code review were approved without requested changes, and its reference note records no implementation blocker.

## Issues open

Nothing open.

## Follow-ups

Task 9.5 will connect this driver to the program entry point and print runtime errors. The remaining phase 09 work includes the error printer and full `main` integration.

## Branch notes

The branch includes the phase 09 planning and architecture/conventions updates carried forward from earlier task work; those planning changes were documented in the prior task PR narrative.

## Where to start reviewing

1. `coders/src/lifecycle/run_simulation.c` — follow the complete lifecycle and cleanup decision path.
2. `docs/plan/phase-09-lifecycle/9.4.1-run-simulation.md` — review the return and ownership contract.
3. `tests/acceptance/test_9.4.1.c` — see the public API scenarios covered by the task.
