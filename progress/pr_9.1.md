## Overview

Task 9.1 adds the shared simulation state needed to distinguish a runtime failure from a normal stop. It introduces the `failed` flag and monitor thread handle, initializes the failure flag, and provides a synchronized operation for marking failure. Later lifecycle work can use this state to coordinate recovery and select the run result.

## Approach

The task establishes the state before adding the wider thread lifecycle. `init_simulation` initializes `failed`; `set_failed` writes it under the existing `stop_mutex`. The setter does not stop the run or wake waiters, leaving those actions to the caller's recovery sequence. The monitor handle remains untouched during initialization and is owned by the later thread-start operation.

## Decisions

The phase plan keeps failure recording separate from stopping and output. The shared flag is synchronized with `stop_mutex`, and the run result is read only after successful joins of every created thread. The monitor thread handle is not initialized to a sentinel because `pthread_t` has no portable sentinel value.

## Issues resolved

**Initialization oracle mismatch.** The first plan review for 9.1.1 required `released_at == 0`, conflicting with the existing initialization contract in 5.4.2 and `init_dongle`. The spec and acceptance oracle were corrected to require `-1`, preserving the established behavior; the plan gate then approved the corrected contract.

**Phase breakdown and file ordering.** The initial Phase 09 review found 9.2.1 combined two function changes and that shared edits to `main.h` and `Makefile` lacked explicit dependency ordering. The phase plan split the coder changes into 9.2.1 and 9.2.2 and added ordering dependencies, including 9.1.2 before 9.3.1 and 9.4.1 before 9.5.1. The revised phase received `PHASE_APPROVED`.

## Issues open

Nothing open.

## Follow-ups

Later lifecycle subtasks call `set_failed` as part of best-effort error recovery, then stop the simulation and wake waiters. The run coordinator joins created threads before reading `failed`; the CLI maps a failed run to its runtime error result in later phase work.

## Branch notes

The task branch also carries the phase 09 planning breakdown and its declared planning/conventions updates from the preceding planning work, as recorded in `progress/current.md`.

## Where to start reviewing

1. `coders/src/state/set_failed.c` — the synchronized failure-state operation.
2. `coders/main.h` — the simulation state and public internal declaration.
3. `coders/src/init/init_simulation.c` — initialization contract from 9.1.1.
4. `tests/acceptance/test_9.1.2.c` — public behavior coverage for the setter.
