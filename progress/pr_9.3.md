## Overview

Task 9.3 adds lifecycle thread coordination: `join_threads` waits for the
monitor and the requested coder threads, while `start_threads` establishes the
simulation clock and launches the monitor before the coders. These operations
provide the thread boundary that the run driver in task 9.4 will use.

## Approach

Both operations live in `lifecycle/` and operate on the initialized
simulation context. Joining is kept separate from shutdown policy: callers
arrange for threads to finish, then join them. Startup sets the shared clock
before creating threads, creates the monitor first, and on partial coder
creation failure marks failure, stops and wakes waiters, then joins only the
threads that exist. Results distinguish recoverable startup failure from a
failed recovery join, where the simulation storage must remain valid.

## Decisions

The approved phase plan follows the user decision to start the monitor first
without a start barrier. Startup uses `0`, `-1`, and `-2` to distinguish full
creation, a safely cleaned-up failure, and a failed recovery join. Thread
creation failures are opportunistically exercised in a constrained child
process; paths that cannot be induced reliably are assigned to code review.

## Issues resolved

**Phase breakdown ordering.** The initial phase review asked for the combined
coder failure changes to be split and for explicit dependencies wherever
subtasks shared production files. The breakdown was revised: 9.2.1 and 9.2.2
became separate contracts, and 9.1.2 now precedes 9.3.1, which precedes
9.3.2. The phase gate then approved the ten-subtask plan.

**Acceptance output capture during 9.3.1.** Adding a lifecycle source file
made the existing 1.1.1 `make re` output exceed its fixed capture buffer, so
that oracle aborted even though the lifecycle implementation was unrelated.
The test was updated to drain through EOF while retaining only bytes that fit;
this kept the output check intact without a source-count limit. The issue and
its resolution are recorded in `progress/current.md` and the 9.3.1 reference
note.

**Stdheader correction during 9.3.2.** The first code review found the new
source header and `Updated:` lines had been typed manually. They were replaced
with generated Stdheader output and rechecked against the plugin output before
the subtask was approved.

## Issues open

Nothing open.

## Follow-ups

Task 9.4 must use the caller-owned simulation storage throughout thread
execution, call `join_threads` after successful startup, and preserve storage
if any join fails. A successful `start_threads` return means all threads were
created, not that they are still running when the call returns.

## Branch notes

## Where to start reviewing

1. `coders/src/lifecycle/start_threads.c` — clock publication, thread creation, and partial-start recovery.
2. `coders/src/lifecycle/join_threads.c` — the shared full and partial join path.
3. `docs/plan/phase-09-lifecycle/9.3.2-start-threads.md` — the return contract, acceptance criteria, and recovery constraints.
4. `tests/acceptance/test_9.3.2.c` — observable lifecycle behavior and opportunistic creation-failure handling.
