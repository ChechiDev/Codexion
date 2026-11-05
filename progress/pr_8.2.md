## Overview

Task 8.2 adds the monitor thread routine that combines the deadline and compile-target checks from task 8.1. It ends the run on completion or burnout, emits one serialized burnout line when needed, and wakes coders waiting on dongles.

## Approach

The monitor repeatedly checks for an existing stop, then checks whether every coder met the target, then scans for the first burned coder. This order gives completion precedence when a scan observes both outcomes. Existing monitor, logging, stop, and dongle APIs own synchronization; the routine coordinates them without directly locking mutexes. A checked sleep of at most 1 ms bounds polling cadence.

## Decisions

The user chose target-first precedence for a joint observation, following the subject's completion wording. The implementation currently reports the lowest-index burned coder when several are due. The spec keeps that choice inside one static helper and leaves alternate tie-breaks for a later defense request.

## Issues resolved

Nothing to note. The phase and subtask plans were approved without requested changes, and the 8.2.1 code review found no blocking issues.

## Issues open

Nothing open.

## Follow-ups

Phase 09 will wire the monitor thread into simulation lifecycle startup and joining. The tie-break may be changed during defense within the selection helper if requested.

## Branch notes

## Where to start reviewing

1. `coders/src/monitor/run_monitor.c` — follow the scan order and terminal paths.
2. `docs/plan/phase-08-monitor/8.2.1-run-monitor.md` — see the sealed contract and acceptance criteria.
3. `tests/acceptance/test_8.2.1.c` — inspect the observable cases for target precedence, burnout, wake-up, and failures.
