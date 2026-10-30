## Overview

Task 4.2 adds the fixed-capacity storage lifecycle for the hand-written binary min-heap used by the FIFO/EDF priority queue. `init_heap` allocates an independently owned request array and leaves the heap unchanged on failure; `free_heap` releases that array and restores the empty state so callers can safely clean up and reuse the struct.

## Approach

The task keeps heap storage in the core layer and exposes it through the shared `t_heap` API. Initialization owns one contiguous array with a fixed capacity, while teardown is a small inverse operation that frees only that array and resets all fields. Heap ordering and operations remain separate responsibilities: `policy/` defines request order, and task 4.3 supplies push, peek and pop.

## Decisions

The specs keep allocation failure handling and implementation ordering in code review, where those details can be checked directly without a non-portable forced-allocation-failure test. Teardown accepts a zeroed heap and NULL pointer, and resetting the fields makes repeated cleanup safe under the documented ownership precondition.

## Issues resolved

No task 4.2 review round requested changes. Both subtask code reviews were approved. The earlier roadmap review correction concerned exact scheduler parsing in Phase 02; it was resolved during roadmap planning and does not change this task.

## Issues open

Nothing open.

## Follow-ups

Task 4.3 builds heap operations on this storage API. The later simulation lifecycle must initialize each heap it may clean up before fallible initialization and must stop/join users before freeing shared heap storage, as specified by the plan.

## Branch notes


## Where to start reviewing

1. `docs/plan/phase-04-priority-queue/4.2.1-init-heap.md` — allocation, ownership and failure contract.
2. `coders/src/heap/init_heap.c` — publishes storage only after successful allocation.
3. `docs/plan/phase-04-priority-queue/4.2.2-free-heap.md` — teardown contract and acceptance criteria.
4. `coders/src/heap/free_heap.c` — releases the owned array and restores the empty state.
