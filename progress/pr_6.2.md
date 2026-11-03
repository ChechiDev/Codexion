## Overview

Task 6.2 adds the two readiness decisions needed by dongle arbitration: a pure
predicate that checks whether a coder may take one dongle, and a clock adapter
that builds an absolute deadline for a bounded condition-variable wait. Together
they let the acquisition work apply queue order and cooldown rules while waiting
without mixing simulation elapsed time with the wall clock used by timed waits.

## Approach

`is_dongle_ready` keeps the per-dongle rule deterministic: its caller supplies
elapsed simulation time and holds the dongle mutex while the function reads the
queue head, availability, and release time. `get_deadline` is an adapter that
reads `gettimeofday` once and returns a normalized absolute `timespec` for the
default `pthread_cond_timedwait` clock. Both functions preserve outputs or state
on invalid input and keep synchronization with their callers.

## Decisions

The phase review clarified that a representable `LONG_MAX` millisecond deadline
must succeed on the 64-bit target; overflow rejection remains conditional on the
actual `time_t` limit. The phase plan also sequences the subtask edits to the
shared header and Makefile.

## Issues resolved

**Large deadline expectation.** The initial deadline acceptance contract treated
`LONG_MAX` milliseconds as an overflow, even though the target's current epoch
plus that duration fits in `tv_sec`. This mismatch was missed because the initial
plan did not account for the division from milliseconds to seconds on the 64-bit
target. Rejecting all very large delays would have contradicted the representable
deadline behavior, so the spec was revised to require success for `LONG_MAX` and
reject only an actual time limit overflow. The phase 06 plan was then approved
with the corrected acceptance criterion and explicit dependency ordering.

**Shared header and build ordering.** The phase breakdown initially lacked a
complete dependency chain for subtasks that edit `coders/main.h` and
`coders/Makefile`; isolated spec reviews did not expose the cross-spec ordering
gap. Parallel independent edits were considered, but explicit sequencing was
chosen to keep each subtask's public declaration and build registration ordered
with its prerequisites. The phase plan added those dependency paths, including
6.2.2 after 6.2.1, and the final phase review approved the acyclic chain.

## Issues open

Nothing open.

## Follow-ups

The later acquisition work uses `get_deadline` for each bounded dongle condition
wait and combines that wait with `is_dongle_ready` under the dongle locks.

## Branch notes

## Where to start reviewing

1. `coders/src/policy/is_dongle_ready.c` — the elapsed-time readiness rule.
2. `coders/src/time/get_deadline.c` — conversion to an absolute timed-wait deadline.
3. `tests/acceptance/test_6.2.2.c` — observable deadline and error behavior.

