# ISS-46 "fix: an expired burnout deadline can be erased before the monitor sees it [8.1]"
- issue: https://github.com/ChechiDev/codexion/issues/46
- type: fix
- origin: 7.1
- priority: now
- goal: an expired burnout deadline is never erased by its coder, and the monitor reports burnout before the compile target, so every burnout is logged as the subject requires

## Issue body (imported from GitHub)

## Problem

A burnout can go unreported.

1. `coders/src/coder/set_compile_start.c:30` overwrites `last_compile_start`
   unconditionally. `coders/src/monitor/get_burned_out.c:25` only sees that
   value. The monitor scans every ~1 ms (`run_monitor.c`, `usleep(1000)`).
   If a coder's deadline `last_compile_start + time_to_burnout` passes just
   after a scan, and the coder gets both dongles and starts compiling before
   the next scan, the new start replaces the expired one. The burnout is never
   detected or logged, and the simulation goes on.
2. `coders/src/monitor/run_monitor.c:68` (`scan_step`) checks the compile
   target before burnout. If both become true between two scans, the target
   stop hides an expired deadline.

Subject clauses: §5.2 `time_to_burnout` (a coder that does not start
compiling within it burns out), §6 rule 7 (burnout logged within 10 ms),
§6 rule 9 (the simulation stops on burnout).

Steps to reproduce: timing dependent, rare in normal runs. It can be forced
deterministically in a test that calls `set_compile_start` on a coder whose
deadline has already passed and then runs one monitor scan.

Expected: the expired deadline is still detected and `X burned out` is logged.
Actual: the coder's new start time hides it and nothing is logged.

## Overview

A coder will not be able to hide its own expired deadline, and the monitor
will give burnout priority over the compile target when both hold.

## Approach

- `set_compile_start` will check, under the coder mutex, whether the current
  deadline has already passed (`last_compile_start <= now - time_to_burnout`,
  the same inclusive rule as `get_burned_out`). If it has, it will not
  overwrite the value and will report it to the caller, so the coder will not
  compile and the monitor will still detect and log the burnout.
- `scan_step` will check burnout before the compile target.

## Decisions

- The inclusive deadline rule of `get_burned_out` (reach == burnout) will be kept.
- The 1 ms monitor period and the log format will not change.
- The work will cover only what the subject requires; nothing extra.

## Open questions

- Return contract of `set_compile_start` when the deadline has expired (new
  return value versus an out flag), and how `run_coder` reacts (it stops
  without compiling and releases its dongles). To be decided in the spec.

## Follow-ups

None expected.

## Subtasks

Planned when the issue starts (`make issue-start`), one spec per subtask.
Likely: (1) coder keeps an expired deadline, (2) monitor checks burnout
first.

## Expected files

| Status | File |
|---|---|
| modified | coders/src/coder/set_compile_start.c |
| modified | coders/src/coder/run_coder.c |
| modified | coders/src/monitor/run_monitor.c |
| modified | coders/main.h |

## Documentation

Specs: 7.1.1 `set_compile_start`, 7.3.1 `run_coder`, 8.2.1 `run_monitor`.
Sealed oracles that may need `migrates:` if the return contract changes:
`test_7.1.1`, `test_7.3.1`, `test_8.2.1`. Reference notes in `docs/reference/` for the same subtasks.

## Branch and merge

`fix/ISS-<n>-<slug>` from `main`, one PR that closes this issue.

## Where to start reviewing

`coders/src/coder/set_compile_start.c`, `coders/src/monitor/run_monitor.c`,
then the new acceptance tests.
<!-- harness-publish-ac5ee6b8-aa63-4d99-88a5-04ab632ca987 -->
