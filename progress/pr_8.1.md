## Overview

Task 8.1 adds two read-only monitor queries: one detects whether an individual coder has reached its burnout deadline, and the other determines whether every coder has met the configured compile target. Together they provide the observations the monitor routine in task 8.2 needs to choose between continuing and ending the simulation.

## Approach

Both queries live in the monitor orchestration module and use the existing coder mutexes to read shared state. The burnout check samples the last compile start and elapsed simulation time under one coder lock, then compares the deadline without overflowing. The target check reads counts one coder at a time, releasing each mutex before moving on. Both report errors through the project's 0/-1 return convention and leave out-parameters untouched on failure. Logging, stopping, and wakeups remain responsibilities of the monitor routine.

## Decisions

The burnout boundary includes the exact deadline, and each query uses its configured values rather than a hardcoded threshold. The target scan treats zero required compiles as already met. These choices follow the subject and the approved subtask specifications.

## Issues resolved

**Target scan coverage and edge cases.** The advisory review found that the initial proposed scan cases did not isolate a below-target final coder, and called for explicit coverage of a zero-target single-coder simulation, negative coder counts, and a high positive target. These gaps could have let a truncated scan or hardcoded threshold pass. The spec was expanded before approval with those cases, additional preservation checks, and explicit instrumentation-aware liveness bounds; the sealed acceptance criteria now cover the first and last positions, `INT_MAX`, zero, invalid counts, and state preservation.

## Issues open

Nothing open.

## Follow-ups

Task 8.2 will consume these queries to implement the monitor loop and own burnout reporting, stopping, and waiter wakeups. Full lifecycle error reporting remains in phase 09 as planned.

## Branch notes


## Where to start reviewing

1. `docs/plan/phase-08-monitor/8.1.1-get-burned-out.md` — the deadline query contract and its timing boundary.
2. `docs/plan/phase-08-monitor/8.1.2-get-target-met.md` — target predicate, synchronization contract, and edge cases.
3. `docs/reference/8.1.1-get-burned-out.md` — the first query's explanation and implementation rationale.
4. `docs/reference/8.1.2-get-target-met.md` — the completed task's second query and its role in monitor policy.
