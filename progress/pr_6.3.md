## Overview

Task 6.3 adds dongle arbitration for acquisition and release. It builds on the request and readiness rules from task 6.1–6.2, then provides stop wake-ups, bounded waiting, cooldown-aware release, atomic pair acquisition, and the single-coder acquisition path. The behavior stays in orchestration and uses the simulation's existing heaps, mutexes, condition variables, and time helpers.

## Approach

Each coder request is built from the shared ticket allocator and FIFO/EDF policy. Pair acquisition queues the same request in both dongle heaps and takes both dongles only when it is head of both queues and both are ready. The one-coder path queues once and holds one mutex. Both paths use bounded condition waits and re-check stop and readiness. Release timestamps cooldown after acquiring the required locks, updates availability, and broadcasts to waiters. Fatal errors after enqueueing stop the simulation and wake waiters because a queued request cannot safely be removed through the public heap contract.

## Decisions

The phase plan assigns the one-coder case a dedicated function because pair acquisition requires distinct dongles. The one-dongle path still uses the shared request and readiness policy so scheduling and cooldown behavior stay consistent. The task keeps user-facing acquisition logging and the one-coder burnout wait for the later coder-flow phase, as specified.

## Issues resolved

**Release cooldown could start before the dongle was actually free.** The phase review found that sampling `released_at` before waiting for contended locks could consume some or all of the cooldown while another thread still owned the dongle. The plan was revised to read elapsed time after acquiring the required mutexes and added an observable contention case; the phase review approved this correction, and the release implementation follows it.

**Pair arbitration's progress oracle read shared state concurrently.** The 6.3.4 progress test checked ownership slots while other worker threads could still write them, creating a ThreadSanitizer race unrelated to production arbitration. The assertion was moved until after all workers were joined, then the oracle was re-sealed. The corrected test retains the mutual-exclusion and completion checks.

**Failed request insertion did not check mutex unlock.** The 6.3.5 code review found that the failed-push cleanup path ignored an unlock failure. The implementation now classifies this as fatal and follows the stop-and-wake path; the subsequent code review approved the fix.

## Issues open

Nothing open.

## Follow-ups

The coder flow in the next phase owns logging after acquisition and waiting for the lone coder to burn out without compiling. Later integration phases still need to exercise the complete run lifecycle.

## Branch notes


## Where to start reviewing

1. `docs/plan/phase-06-dongle-arbitration/6.3.4-take-dongles.md` — the pair request and head-of-both contract.
2. `coders/src/dongle/take_dongles.c` — the two-resource arbitration loop.
3. `coders/src/dongle/take_single_dongle.c` — the one-mutex variant and its fatal path.
4. `coders/src/dongle/release_dongles.c` — cooldown timestamping and waiter notification.
