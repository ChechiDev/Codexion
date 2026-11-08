## Overview

Task 9.2 carries runtime failure information from the coder and monitor paths into the simulation's synchronized failure state. Callers can distinguish a normal stop, such as reaching the compile target or a reported burnout, from an error that should make the overall run fail.

## Approach

The acquisition wrapper preserves the original print result through resource release, so a real output error remains an error even if the stop state changes concurrently. The coder routine carries helper outcomes through its cycle and centralizes cleanup: normal stops release held dongles and return, while failures are recorded before release, stop, and wake-up. The monitor classifies its scan result and uses distinct silent, clean, burnout, and error exits. Both routines use existing state, logging, and dongle APIs; no new public API was needed.

## Decisions

The approved phase contract keeps target completion, zero required compiles, and logged burnout as normal outcomes. Runtime failures record `failed` before requesting stop and wake-up. The task retains the phase invariant that the monitor is the sole thread that stops a production run for a normal reason; other stop requests are part of failure handling.

## Issues resolved

**The first 9.2.1 plan review found no acceptance test that distinguished preserving the print error from the existing behavior.** Its visible consequence was that the proposed oracle would pass before the change, since it did not cover a print failure followed by a stop before classification. A timing race would not make a reliable test, so the plan was amended with AC5: a test-defined `write` hook deterministically causes the stop during the failed write and checks the public return value, untouched output parameter, and resource release. The revised plan gate approved that criterion, and the implementation preserves the original print result through cleanup.

**The phase review split the original combined coder-reporting work into separate function contracts.** Treating dongle-log result preservation and coder-cycle failure handling as one unit would have coupled changes in separate orchestration routines and made review less focused. The phase breakdown assigned the acquisition wrapper to 9.2.1 and `run_coder` to 9.2.2, with separate acceptance paths and an explicit dependency. The approved breakdown is reflected in the task's three subtask specs.

**The 9.2.2 code review requested a helper rename to follow the project's allowed verb list.** The helper name `end_run` did not follow the naming convention, so it was changed to `release_run` at its definition and call sites. No behavior changed, and the follow-up review approved the code.

## Issues open

Nothing open.

## Follow-ups

Task 9.3 joins and starts the worker threads. The lifecycle invariant established here requires startup failure handling to record `failed`, and requires the worker threads to be joined before simulation resources are released.

## Branch notes

## Where to start reviewing

1. `coders/src/coder/take_coder_dongles.c` — follows the original log result through the one-time resource release.
2. `coders/src/coder/run_coder.c` — shows how the coder classifies normal stop and runtime failure while cleaning up.
3. `coders/src/monitor/run_monitor.c` — shows how monitor outcomes map to the four exit paths.
4. `docs/plan/phase-09-lifecycle/9.2.3-monitor-reports-failure.md` — contains the final monitor contract and review check.
