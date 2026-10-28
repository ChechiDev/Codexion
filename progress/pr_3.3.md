## Overview

Task 3.3 adds the time adapter that lets workers wait for a duration while
remaining responsive to the shared simulation stop flag. It builds on the clock
and stop-state APIs from tasks 3.1 and 3.2 and provides a small runtime
primitive for later simulation work.

## Approach

`sleep_ms` measures elapsed time with the shared time API and sleeps in slices
of at most 500 microseconds. Between slices it checks the stop-state reader,
then re-reads elapsed time rather than assuming the requested sleep interval
was fully observed. It holds no simulation mutex during `usleep`, leaving the
stop writer free to make shutdown visible.

## Decisions

The approved contract makes nonpositive durations immediate successes and
uses `-1` for either an observed stop or clock failure. A stop check precedes
the duration check, so an observed stop takes precedence. Slice duration is
bounded, while actual wake-up and stop response timing remains subject to
scheduling.

## Issues resolved

The first code review identified possible signed overflow when subtracting a
negative elapsed value after a backward wall-clock adjustment. The implementation
now uses `get_elapsed_ms`, whose result is nonnegative or `-1`, and subtracts
only after confirming the duration remains. The approved code review records
this fix and found no blocking findings.

## Issues open

Wall-clock time can move backward and extend a wait; the timing contract assumes
no backward adjustment during the interval. The 500 microsecond request does
not guarantee a hard stop-response deadline under scheduling delays.

## Follow-ups

The slice size's wake-up and stop-check cost is assigned to phase 10 for
measurement. The end-to-end burnout deadline remains owned by the monitor work
in phase 08; this adapter alone does not promise that deadline.

## Branch notes

## Where to start reviewing

1. `docs/plan/phase-03-time-and-log/3.3.1-sleep-ms.md` — defines the API,
   timing contract, and acceptance criteria.
2. `coders/src/time/sleep_ms.c` — shows the stop-aware loop and bounded slice.
3. `coders/src/time/get_elapsed_ms.c` — shows the nonnegative elapsed-time
   behavior used to keep remaining-time arithmetic safe.
4. `tests/acceptance/test_3.3.1.c` — exercises duration, immediate return, and
   concurrent stop behavior.
