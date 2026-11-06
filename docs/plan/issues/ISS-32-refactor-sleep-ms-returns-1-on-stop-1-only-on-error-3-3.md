# ISS-32 "refactor: sleep_ms returns 1 on stop, -1 only on error [3.3]"
- issue: https://github.com/ChechiDev/codexion/issues/32
- type: refactor
- origin: 3.3
- priority: now
- goal: sleep_ms returns 0 when the time elapsed, 1 when the simulation was stopped and -1 only on a clock failure, so callers can tell a normal end from a real error, with no change in coder behaviour

## Issue body (imported from GitHub)

## Problem

`sleep_ms` (subtask 3.3.1) returns -1 both when the simulation is stopped
and when the clock fails (`coders/src/time/sleep_ms.c`). Its callers cannot
tell a normal end from a real error. `run_coder` treats any non-zero return as
a failure and goes through `set_failure`.

Phase 09 will add a synchronized `failed` flag so runtime failures make the
program exit non-zero (phase 09 planning decision, 2026-11-06). With the
current contract, every coder that is sleeping when the run ends normally
(burnout or target met) would mark `failed`, and a correct run would exit with
a runtime error.

## Overview

`sleep_ms` will return a three-way result: 0 when the full time elapsed, 1
when the simulation was stopped, -1 on a clock failure. A lock or unlock
failure of `stop_mutex` inside `is_stopped` will still be reported as stopped
(1): that is the existing, sealed contract of `is_stopped` (3.2.1 AC3,
"failed lock reports stopped"), already documented as an accepted limit in
8.2.1, and such a failure cannot be forced with ordinary inputs. This issue
will not change `is_stopped`. `run_coder` will keep its current behaviour: it
will still leave on 1 and on -1. Telling the two apart (marking `failed` only
on -1) will be phase 09 work and is out of scope here.

## Approach

- `sleep_ms` will return 1 on stop and keep -1 for clock failures only.
- The `main.h` Time section will be touched only if its comment needs it (the
  signature will not change).
- `coders/src/coder/run_coder.c` will not be modified: its four uses of
  `sleep_ms` (lines 36, 51, 55, 60) already treat any non-zero result alike
  (compile/debug/refactor sleeps leave on non-zero, the single-coder wait
  loops while the result is 0), so 1 and -1 keep the same coder behaviour.
  (Planning note added at issue start, replacing the earlier plan to edit
  them.)
- Acceptance strategy: 0 after the full time; 1 on an already stopped
  simulation and on a stop during the sleep (within the existing 10 ms
  bound); `ms <= 0` will still return 0. The clock-failure -1 and the
  `is_stopped` lock-failure path will have no acceptance test (not forceable,
  no fault-injection build): code review.
- The sealed oracle `tests/acceptance/test_3.3.1.c` (AC3 and AC4 expect -1 on
  stop) will be migrated, and the unit test `tests/unit/test_sleep_ms.c`
  updated.

## Decisions

- The result will be three-way (0 / 1 / -1) instead of a separate "stopped"
  query, so the check and the result will come from the same call with no
  race (user decision 2026-11-06, option (a) over checking `is_stopped` in
  `set_failure`).
- The change will be done as an issue, not as a phase 09 subtask, because it
  will change the sealed oracle of a closed subtask (`migrates:` is only
  allowed for issue subtasks).
- Priority `now`: phase 09 subtask 9.2.1 will depend on the new contract.
- `is_stopped` will be kept as is: a `stop_mutex` failure will stay
  indistinguishable from a stop and `sleep_ms` will report it as 1. Changing
  it would migrate a second sealed oracle (3.2.1) and every `is_stopped`
  caller for a failure that has no forceable test; it will stay a documented
  accepted limit.

## Open questions

- None for the contract. Whether `run_coder` will mark `failed` on -1 will be
  decided in phase 09.

## Follow-ups

- Phase 09, 9.2.1: `run_coder` will call `set_failed` only on a real -1.

## Subtasks

Planned when the issue starts (`make issue-start`), one spec per subtask.
Expected: one subtask (contract + call sites + oracle migration).

## Expected files

| Status | File |
|---|---|
| modify | `coders/src/time/sleep_ms.c` |
| unchanged (regression-checked) | `coders/src/coder/run_coder.c` |
| modify | `coders/main.h` (only if the Time section comment needs it) |
| migrate | `tests/acceptance/test_3.3.1.c` |
| modify | `tests/unit/test_sleep_ms.c` |

## Documentation

- The sealed spec `docs/plan/phase-03-time-and-log/3.3.1-sleep-ms.md` will
  stay unchanged as a historical record; the new issue spec will state that it
  supersedes AC3 and AC4 of 3.3.1.
- Only the sealed acceptance test `tests/acceptance/test_3.3.1.c` will be
  migrated (listed in the issue spec's `migrates:`).
- The reference `docs/reference/3.3.1-sleep-ms.md` will be updated at close.

## Branch and merge

`refactor/ISS-<n>-sleep-ms-stop-result` from `main`, one PR that closes this issue.

## Where to start reviewing

`coders/src/time/sleep_ms.c`, then the call sites in `coders/src/coder/run_coder.c`.
<!-- harness-publish-d188fc6a-1042-457c-8736-799d25e14147 -->
