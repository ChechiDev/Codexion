## Overview

Task 4.3 completes the priority queue operations needed to manage dongle
requests. It adds insertion, minimum lookup, and removal to the hand-written
binary min-heap, with all ordering delegated to the FIFO/EDF request
comparator. The heap now provides the operations later arbitration needs while
keeping synchronization with its callers.

## Approach

The heap stores requests in caller-owned fixed-capacity array storage. Push
copies a request into the next slot and sifts it upward; head lookup copies the
root without changing the heap; pop copies the root out, moves the final
occupied entry to the root, and sifts it downward. Each operation uses the
shared `(key, ticket)` order, so FIFO and EDF policy stay outside heap
mechanics. Invalid and rejected operations preserve their documented outputs
and heap state.

## Decisions

The approved specs keep locking with callers and use no remove-by-coder
operation. Later arbitration can check both dongle heap heads and remove the
request only when it is eligible in both queues.

## Issues resolved

No task 4.3 review round requested changes. The roadmap had earlier review
rounds about scheduler input matching and phase scope; those were resolved
before the roadmap and phase 04 received approval and did not require changes
to task 4.3.

## Issues open

Nothing open.

## Follow-ups

Phase 06 must hold the relevant dongle mutexes while inspecting and changing
their heaps, following the approved lower-index-first lock order and
head-of-both rule. Simulation state ownership and teardown are specified for
phase 05; arbitration and cooldown behavior follow in phase 06.

## Branch notes

## Where to start reviewing

1. `coders/src/policy/is_request_before.c` — establishes the shared key and
   ticket ordering used by every heap operation.
2. `coders/src/heap/push_request.c` — shows how requests enter and rise through
   the queue.
3. `coders/src/heap/pop_request.c` — shows removal and restoration of heap order.
4. `coders/main.h` — contains the public heap API declarations.
