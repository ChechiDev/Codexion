---
id: ISS-46.1
alias:
  - Keep expired deadline
  - Preserve coder burnout deadline
tags:
  - codexion
  - coder-thread
  - burnout
  - synchronization
date: 2026-11-12 11:18
---

<!-- section: concept -->
## Preserving a reached burnout deadline

`last_compile_start` is also the monitor's evidence that a coder has exceeded its allowed time without beginning another compile. If the coder acquires its dongles after that deadline has already been reached, replacing the timestamp with the current time would erase that evidence before the monitor can see it.

`set_compile_start` now makes the decision while holding the coder mutex. It uses the same inclusive rule as `get_burned_out`: the deadline is reached when `last_compile_start <= now - time_to_burnout`.

| Deadline at the call | Result | Timestamp | Caller effect |
|---|---:|---|---|
| Not reached | `0` | Replaced by current elapsed time | Compile may proceed |
| Reached, including exact equality | `1` | Kept unchanged | Routine ends; dongles are released and no compile is logged |
| Invalid input or lock, clock, or unlock failure | `-1` | As defined by the failure point | Error path |

Keeping the deadline lets the monitor remain responsible for detecting and reporting a normal burnout. The caller already treats a nonzero result as a reason to leave the routine; result `1` therefore stops this compile attempt without setting the simulation's failed or stopped state.

<!-- section: planning -->
## How it was planned

ISS-46.1 isolates the coder-side race from ISS-46.2's monitor scan order. The approved spec changes only `set_compile_start`'s contract and declares migration of the sealed 7.1.1 acceptance oracle. Seven acceptance criteria cover a live deadline, a passed deadline, inclusive equality, monitor visibility, caller cleanup without compilation, unchanged invalid and clock-clamp paths, and the build symbol plus Norm. Lock, clock, and unlock failures are checked by source review because they cannot be forced deterministically.

<!-- section: execution -->
## How it was executed

The implementation reads elapsed time and checks the deadline under the coder mutex, keeping the check and any timestamp update in one critical section. A reached deadline leaves the field untouched and returns `1`; otherwise the current elapsed time is stored and the function returns `0`. The existing `run_coder` path already ends the routine and releases held dongles for a nonzero result, so it needed no change. The unit test covers both outcomes and mutex release.

<!-- section: problems -->
## Problems & resolution

The first code review found that the unit test's live-deadline case had only a 1 ms margin, so ordinary scheduling delay could make the correct implementation return the expired result. The test now uses a 200 ms margin while retaining the exact inclusive-boundary assertion; the follow-up code review approved the change.

<!-- section: files -->
## Files created / modified

- [[coders/src/coder/set_compile_start.c]] — preserves a reached deadline and returns the documented 0 / 1 / -1 result.
- [[tests/unit/test_set_compile_start.c]] — covers preserved deadlines, the inclusive boundary, the live path, and mutex release.
- [[tests/acceptance/test_ISS-46.1.c]] — exercises the seven approved acceptance criteria.
- [[tests/acceptance/test_7.1.1.c]] — migrates live-deadline prefills to the new contract.
- [[docs/plan/issues/ISS-46-fix-an-expired-burnout-deadline-can-be-erased-before-the-monitor-sees-it-8-1/ISS-46.1-keep-expired-deadline.md]] — records the approved subtask and its completed status.
- [[docs/plan/PLAN.md]] — regenerated issue index.
- [[feature_list.json]] — regenerated plan index.
- [[progress/current.md]] — records the close and next subtask.
- [[progress/history.md]] — appends the close record.
- [[progress/close_ISS-46.1.msg]] — commit message for the harness close.
