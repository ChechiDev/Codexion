## Overview

Task 3.1 adds the clock foundation for the runtime: an epoch timestamp in milliseconds and elapsed milliseconds relative to a shared start. Later time, sleep, and logging work can use the same clock API, while the elapsed-time function remains independent of simulation state.

## Approach

The system wall clock is read in one adapter, `get_time_ms`, and `get_elapsed_ms` derives a relative timestamp from that API. The elapsed function checks the clock failure sentinel before subtracting and clamps a future start to zero. Both functions expose their values directly, with `-1` reserved for clock failure.

## Decisions

The accepted clock contract returns a value or `-1` rather than using a status and output parameter. This keeps the common time path small while callers still check the failure sentinel before arithmetic or output. Elapsed time takes a plain start timestamp rather than `t_sim`, so it needs no shared state.

## Issues resolved

The roadmap review initially requested restoring exact `fifo`/`edf` matching in the Phase 02 planning documents; the roadmap was then approved with that correction. The roadmap was also re-gated after plan seal changes. Neither issue changed task 3.1's implementation. The phase 03 and task 3.1 reviews recorded no requested changes.

## Issues open

Wall-clock adjustments can move readings backwards. The elapsed-time contract clamps future starts, and timing acceptance criteria state the no-backward-adjustment assumption for bounded intervals.

## Follow-ups

Subsequent phase 03 tasks build on this API for stop-aware sleeping and serialized log timestamps. Time remains wall-clock based, as specified.

## Branch notes

This branch includes the approved phase 03 plan carried by the 3.1.1 close commit.

## Where to start reviewing

1. `docs/plan/phase-03-time-and-log/3.1.1-get-time-ms.md` — establishes the wall-clock value and failure contract.
2. `coders/src/time/get_time_ms.c` — implements the epoch-millisecond adapter.
3. `docs/plan/phase-03-time-and-log/3.1.2-get-elapsed-ms.md` — defines relative-time behavior and edge cases.
4. `coders/src/time/get_elapsed_ms.c` — derives elapsed time and applies the failure and future-start rules.
