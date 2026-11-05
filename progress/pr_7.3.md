## Overview

Task 7.3 adds the thread routine that runs one coder through its compile, debug, and refactor cycle. It also handles the one-coder case, where a coder holds its single dongle and waits for the simulation to stop. The routine coordinates the existing coder state, dongle, time, logging, and stop APIs.

## Approach

`run_coder` composes the established operations in cycle order: acquire and log dongles, record the compile start, log and wait for compiling, count only a completed compile, release the dongles, then log and wait through debugging and refactoring. A stop check after refactoring also terminates zero-duration cycles. In the one-coder case it acquires and logs the shared dongle once, then uses interruptible sleeps until stop. Local helpers keep the cycle and exit paths small, while synchronization remains with the dependency that owns each field or resource.

## Decisions

The routine uses one exit path for dependency failures because stop-aware operations and real failures both report `-1`; it attempts release only while its local ownership flag is set, then calls both stop and wake. It always returns `NULL`, leaving error reporting for lifecycle integration in phase 09. These contracts follow the approved spec and its advisory review.

## Issues resolved

**Resource ownership after dependency failures.** The spec initially needed to account for dependencies that may fail after changing resource state, which affects whether the caller can safely release. Retrying or releasing based on an assumption about internal state could double-release or act on an unowned dongle, and this boundary was not visible from the public return value alone. The reviewed options were to retry cleanup, inspect dependency internals, or keep ownership in the routine's local state and defer unknown partial failures to the dependency contract. The approved spec chose the last option: clear the ownership flag before each release attempt, attempt release at most once, and always run the stop/wake policy. This is implemented in `run_coder.c` and described in the subtask reference note.

**Ambiguous stop and failure results.** `print_state` and `sleep_ms` return `-1` both when stopping and on operational errors, so separate branches could leave neighboring coders waiting after a genuine failure. The spec considered treating these returns as normal stop only, but that would hide real failures; reporting a distinct thread result was outside the public `pthread_create` contract and lifecycle scope. The common exit now performs best-effort release, `set_stopped`, and `wake_dongles` for every `-1`; repeated stop operations are safe for an already stopped run. The code review approved this policy, with differentiated lifecycle reporting left to phase 09.

**Observable cycle and timing criteria.** The initial acceptance design needed to prove thread progress and prompt stopping without assuming a fixed scheduler runtime, and to distinguish debug and refactor timing. Fixed-duration expectations and assertions about the exact stop check were considered too brittle because scheduling and zero-length sleeps affect which operation observes stop. The final ACs observe completed refactor progress, require the distinct minimum intervals, and test both long and zero-duration stop paths; implementation-specific clock and synchronization failures remain source-review checks. These refinements are recorded in `progress/review_history/7.3.1.md` and the approved spec.

## Issues open

The thread routine always returns `NULL`, so the caller cannot distinguish a normal stop from a dependency failure by its return value. Phase 09 owns lifecycle error reporting.

## Follow-ups

Phase 08 adds the monitor that detects burnout or completion and initiates stop. Phase 09 can add lifecycle handling that distinguishes a routine failure from normal termination.

## Branch notes

## Where to start reviewing

1. `coders/src/coder/run_coder.c` — the cycle, ownership tracking, and common exit policy.
2. `coders/src/coder/take_coder_dongles.c` — the acquisition and per-dongle logging contract used by the routine.
3. `docs/plan/phase-07-coder-thread/7.3.1-run-coder.md` — the acceptance criteria and source-review requirements.
