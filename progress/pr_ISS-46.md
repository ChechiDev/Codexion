## Overview

ISS-46 fixes two paths that could hide a coder's expired burnout deadline. The coder now preserves an already-reached deadline when attempting to begin another compile, and the monitor reports burnout before stopping for a compile target observed in the same scan. Together these changes keep the monitor responsible for detecting and logging burnout under the subject's timing and stop requirements.

## Approach

ISS-46.1 makes the deadline check and timestamp update one critical section under the coder mutex. It returns a distinct result when the deadline has expired; the existing caller ends that routine and releases its dongles without setting failure or stopping the simulation. ISS-46.2 changes only the monitor scan order: external stop, burnout, then target. Existing helpers continue to select and report the burned-out coder and wake waiters. Acceptance oracles cover the new contracts and migrate only assertions that encoded the old behavior.

## Decisions

The user chose burnout-first when burnout and target are both observed in one scan, following the advisory that neither ordering can reconstruct exact chronology and that burnout-first avoids hiding a reached deadline. The inclusive deadline rule and the 1 ms monitor period remain unchanged. The zero-required application case remains silent because simulation setup returns before starting threads.

## Issues resolved

**Missed sealed-oracle migration.** The first ISS-46.2 migration list omitted `test_9.2.3.c` AC2, which expected a silent stop for a synthetic zero-required monitor state. The implementation made that assertion fail. The subtask migration scope was corrected to include AC2 with the same burnout expectation used by the affected 8.2.1 cases; the sealed historical spec remains unchanged. The issue review history records this omission and the corrected scope.

**Issue re-gate cycle.** After the omitted oracle was identified, the issue gate could not approve while its supplied check was red, and the subtask plan gate could not proceed without the issue approval. The workflow was reset and the issue gate was rerun; the issue review documents that the ordinary check was red only because the completed ISS-46.1 and active ISS-46.2 lacked the renewed issue verdict, while the counterfactual check assuming that verdict passed. The final issue verdict is `ISSUE_APPROVED`, after which the plan and code gates completed.

**Legacy header refresh.** The first ISS-46.2 code review requested the 42 header `Updated:` field be refreshed under the project conventions. The header was refreshed and the next code review approved the change.

## Issues open

The Valgrind run of migrated `test_8.2.1.c` hit a timing assertion in AC1. The implementation report records that the same assertion fails against the committed baseline under Valgrind, identifying slowdown sensitivity rather than a regression. No blocking review findings remain.

## Follow-ups

The target and burnout checks use the state visible in each monitor scan; they do not retain event timestamps for reconstructing which condition became true first between scans. Burnout-first is the selected conservative tie-break. No follow-up work is specified.

## Branch notes

## Where to start reviewing

1. `coders/src/coder/set_compile_start.c` — see how an expired deadline is preserved and signaled to the caller.
2. `coders/src/monitor/run_monitor.c` — see the final stop, burnout, and target precedence.
3. `tests/acceptance/test_ISS-46.1.c` and `tests/acceptance/test_ISS-46.2.c` — follow the new coder and monitor contracts.
4. `tests/acceptance/test_7.1.1.c`, `tests/acceptance/test_8.2.1.c`, and `tests/acceptance/test_9.2.3.c` — review the narrowly migrated sealed expectations.
