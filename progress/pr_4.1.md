## Overview

Task 4.1 establishes the request ordering used by the priority queue: requests compare by policy key and then arrival ticket, while a separate pure policy function computes FIFO ticket keys or EDF deadlines. It provides the ordering contract that the heap work in task 4.2 and arbitration work in later phases can share.

## Approach

The task keeps policy decisions in the core `policy/` module. `is_request_before` implements strict lexicographic ordering by `(key, ticket)`; `get_request_key` produces the key for either mode without introducing clock, lock, allocation, output, or shared-state dependencies. The comparator handles ties, so EDF key calculation does not incorporate the ticket. The public functions are declared in the single project header and registered in the deliverable Makefile.

## Decisions

The approved plan uses `-1` as the key-calculation error result because a valid key is nonnegative and the four-parameter Norm limit rules out adding an out parameter. EDF overflow is rejected before addition rather than saturated, preserving deadline ordering. FIFO ignores its time arguments.

## Issues resolved

The roadmap review first requested restoration of exact `fifo`/`edf` scheduler values, and a later roadmap round required the roadmap/phase plan approval and seals to be resolved through the gate workflow. The approved roadmap records those outcomes. The task 4.1 plan and code reviews for both subtasks requested no changes; no task-level implementation issues remained unresolved.

## Issues open

Nothing open.

## Follow-ups

Task 4.2 supplies heap storage and operations using this request ordering. Later arbitration callers must check `get_request_key` for `-1` before enqueueing, preserve the same `(key, ticket, coder)` request in both dongle queues, and follow the shared elapsed-millisecond time base for EDF.

## Branch notes


## Where to start reviewing

- `coders/src/policy/is_request_before.c` — establishes key-first, ticket-second ordering.
- `coders/src/policy/get_request_key.c` — maps FIFO and EDF inputs to validated keys.
- `docs/plan/phase-04-priority-queue/4.1.1-is-request-before.md` and `4.1.2-get-request-key.md` — show the approved contracts and acceptance scope.
- `docs/reference/4.1.1-is-request-before.md` and `4.1.2-get-request-key.md` — explain the resulting behavior and design contracts.
