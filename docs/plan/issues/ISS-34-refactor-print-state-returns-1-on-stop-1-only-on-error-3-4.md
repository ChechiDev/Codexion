# ISS-34 "refactor: print_state returns 1 on stop, -1 only on error [3.4]"
- issue: https://github.com/ChechiDev/codexion/issues/34
- type: refactor
- origin: 3.4
- priority: now
- goal: print_state returns 0 when the line was written, 1 when the simulation was stopped (nothing written) and -1 only on a real failure, so callers can tell a normal end from an error, with no change in caller behaviour

## Issue body (imported from GitHub)

## Problem

`print_state` (subtask 3.4.3) returns -1 both when the simulation is already
stopped (the line is suppressed on purpose) and on a real failure (invalid
state or coder, lock or unlock failure, clock or `write` failure)
(`coders/src/log/print_state.c`). Its callers cannot tell a normal end from a
real error.

Phase 09 will add a synchronized `failed` flag so runtime failures make the
program exit non-zero (phase 09 planning decision, 2026-11-06). `run_coder`
calls `print_state` before compiling, debugging and refactoring; when the run
ends normally right before one of those lines, the suppressed line returns
-1, so the coder would mark `failed` on a correct run. Issue #32 already gave
`sleep_ms` the three-way contract for the same reason.

## Overview

`print_state` will return a three-way result: 0 when the line was written, 1
when the simulation was stopped (nothing written), -1 on any failure
(rejected input, lock or unlock failure, clock, build or `write` failure). A
lock or unlock failure of `stop_mutex` while reading the flag will stay -1
(it is a failure, and `print_state` reads the flag itself, not through
`is_stopped`). No caller will change behaviour: every caller still leaves on
any non-zero result. Telling 1 from -1 in `run_coder` will be phase 09 work
(9.2.1).

## Approach

- In `print_state`, the "stopped" branch will return 1 instead of -1; every
  other path keeps its current result. An invalid state or coder will stay
  -1 even on a stopped simulation: the coder check (today only in
  `build_log_line`, after the stop read) will move before any lock, next to
  the state check (planning note added at issue start).
- `coders/src/coder/run_coder.c` and `coders/src/coder/take_coder_dongles.c`
  will not be modified: their uses (`run_coder.c` lines 34, 49, 53;
  `take_coder_dongles.c` line 26) already treat any non-zero result alike.
  In `take_coder_dongles`, a 1 still goes through `release_taken`, which
  releases the dongles and returns 0 with `taken = 0` when the run is
  stopped, exactly as a -1 does today.
- The `print_state` comment in `print_state.c` will describe the new
  contract.
- Acceptance strategy: 0 and one line on a running simulation; 1 and nothing
  written for every state on a stopped simulation; 1 for every call after a
  concurrent stop (the post-stop batch); -1 unchanged for rejected input, lock
  failures and the `/dev/full` write failure; a regression run of
  `run_coder` threads through its public API with a stop. Clock and build
  failures stay code review (not forceable).
- Sealed oracles that pin -1 after a stop will be migrated (only those
  assertions): `tests/acceptance/test_3.4.3.c` (AC4 at line 255; AC7 at the
  worker loop stop condition near line 436, which will break on result 1
  instead of -1, and the post-stop assertion at line 445),
  `tests/acceptance/test_3.4.4.c` (line 294; the worker loop stop condition
  at line 410, which will break on result 1 instead of -1; line 420),
  `tests/acceptance/test_3.4.5.c` (line 69) and
  `tests/acceptance/test_3.4.6.c` (line 278). The unit test
  `tests/unit/test_print_state.c` (stopped case at line 101) will be updated.

## Decisions

- Same three-way contract as `sleep_ms` (#32), so the two functions that
  can observe the stop report it the same way (user decision 2026-11-06,
  option (1) over re-checking `is_stopped` after a -1 in `run_coder`, which
  would lose a real `write` error that coincides with a stop).
- Done as an issue, not as a phase 09 subtask, because it changes the sealed
  oracles of closed subtasks (`migrates:` is only allowed for issue
  subtasks).
- Priority `now`: phase 09 subtask 9.2.1 will depend on the new contract.
- `print_burnout` (3.4.4) will not change: it already distinguishes a
  refused burnout line by its own contract and is used only by the monitor.

## Open questions

- None for the contract.

## Follow-ups

- Phase 09, 9.2.1: `run_coder` will call `set_failed` only on a real -1 from
  `print_state` or `sleep_ms`.

## Subtasks

Planned when the issue starts (`make issue-start`), one spec per subtask.
Expected: one subtask (contract + oracle migrations).

## Expected files

| Status | File |
|---|---|
| modify | `coders/src/log/print_state.c` |
| unchanged (regression-checked) | `coders/src/coder/run_coder.c` |
| unchanged (regression-checked) | `coders/src/coder/take_coder_dongles.c` |
| migrate | `tests/acceptance/test_3.4.3.c` |
| migrate | `tests/acceptance/test_3.4.4.c` |
| migrate | `tests/acceptance/test_3.4.5.c` |
| migrate | `tests/acceptance/test_3.4.6.c` |
| modify | `tests/unit/test_print_state.c` |

## Documentation

- The sealed specs of 3.4.3, 3.4.4, 3.4.5 and 3.4.6 will stay unchanged as
  historical records; the new issue spec will state which of their criteria
  it supersedes.
- Only the four sealed acceptance tests listed above will be migrated (listed
  in the issue spec's `migrates:`).
- The reference `docs/reference/3.4.3-print-state.md` will be updated at
  close.

## Branch and merge

`refactor/ISS-<n>-print-state-stop-result` from `main`, one PR that closes this issue.

## Where to start reviewing

`coders/src/log/print_state.c`, then the migrated assertions in the four
acceptance tests.
<!-- harness-publish-45d4e8dd-9114-48a6-b5cd-ffde4e147968 -->
