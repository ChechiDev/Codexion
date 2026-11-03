# Phase 06 — Dongle arbitration
- goal: atomic two-dongle acquisition (a coder takes both only when it is the head of both dongle heaps and both are available with cooldown elapsed, mutexes locked lower-index first), release with cooldown, pthread_cond_timedwait waits with no busy-wait and wake-all on stop, with arbiter-level tests (head-of-both rule, fifo/edf ordering, cooldown, stop wake-up) as evidence for the PLAN.md progress argument

## 6.1 Requests
- layer: adapters
- goal: hand out one simulation-wide, strictly increasing ticket per request under its mutex, and build a coder's `(key, ticket, coder)` request for the configured policy before any dongle is locked

## 6.2 Readiness
- layer: adapters
- goal: decide whether one dongle can be taken by a coder (head of its heap, available, cooldown elapsed on the simulation clock) and build the absolute deadline that bounds a timed wait

## 6.3 Acquisition and release
- layer: orchestration
- goal: take both dongles at once (same request in both heaps, lower index locked first, pop both in one critical section) or the single dongle when there is one coder, wait with `pthread_cond_timedwait` and no busy-wait, release with cooldown and wake-up, wake every waiter on stop, and end the run cleanly on any failure after a request is queued
