---
id: ISS-32.1
alias:
  - Sleep stop result
tags:
  - time
  - sleep
  - stop-state
date: 2026-11-06 12:10
---

<!-- section: concept -->
## Distinguishing a stop from a clock error

`sleep_ms` waits in short slices and checks the shared stop state between
slices. Its result now says why the wait ended:

| Result | Meaning |
|---:|---|
| `0` | The requested duration elapsed, or `ms <= 0` required no wait. |
| `1` | The simulation was observed stopped. |
| `-1` | A clock reading failed. |

For example, `sleep_ms(sim, 100)` on a running simulation returns `0` after
roughly 100 ms. If another thread calls `set_stopped` while
`sleep_ms(sim, 10000)` is waiting, the next stop check returns `1`. A failed
clock read remains `-1`. The stop check still runs before the elapsed-time
check, so an observed stop wins if both conditions are true in the same loop
iteration.

This distinction gives later callers a way to treat normal shutdown separately
from an actual time-adapter failure. Existing `run_coder` callers already leave
on any nonzero result, so the new value preserves their behavior. A failure in
the underlying `is_stopped` mutex operation remains reported as stopped because
that is the established `is_stopped` contract.

<!-- section: planning -->
## How it was planned

The approved issue spec scopes the behavior change to `sleep_ms` in the time
adapter, with the shared header listed only if its contract comment needs an
update. Acceptance criteria cover nonpositive and completed waits, an already
stopped simulation, a concurrent stop, coder behavior through `run_coder`, and
the build, symbol, and Norm checks. The sealed 3.3.1 acceptance oracle is
migrated only for its stop-result expectations; the historical phase spec stays
unchanged. Clock failure is not forceable through ordinary public inputs, so
its `-1` behavior is checked by source review.

<!-- section: execution -->
## How it was executed

The implementation changes the return value at the existing stop branch from
`-1` to `1`. Initial and loop clock failures remain `-1`; nonpositive
durations, stop-first polling, bounded sleep slices, elapsed-time handling, and
the treatment of interrupted `usleep` calls remain as before. No caller change
was needed: each `run_coder` sleep use already treats any nonzero result as a
reason to leave. The unit and acceptance coverage pin both the new result and
the unchanged coder shutdown behavior.

<!-- section: problems -->
## Problems & resolution

The first issue review found two planning gaps: the header promised edits to
`run_coder.c` even though existing callers already handled either nonzero
result, and the proposed coder regression could pass without a coder running.
The issue plan was aligned with the no-caller-edit scope and replaced that check
with a public-API run against an initialized simulation that requires real
compile progress. The review also moved sealed-oracle migration assertions out
of acceptance criteria and into migration instructions. The revised issue was
approved; implementation and code review required no further changes.

<!-- section: files -->
## Files created / modified

- [[coders/src/time/sleep_ms.c]] — reports an observed stop as `1`, preserving `-1` for clock failures.
- [[tests/unit/test_sleep_ms.c]] — expects the stop result for stopped waits.
- [[tests/acceptance/test_ISS-32.1.c]] — covers the public return contract, stop timing, coder behavior, and build integration.
- [[tests/acceptance/test_3.3.1.c]] — migrates the prior stop-result assertions from `-1` to `1`.
- [[docs/plan/issues/ISS-32-refactor-sleep-ms-returns-1-on-stop-1-only-on-error-3-3/ISS-32.1-sleep-stop-result.md]] — records the approved scope and close status.
- [[docs/plan/PLAN.md]] and [[feature_list.json]] — regenerated plan indexes.
- [[progress/current.md]] and [[progress/history.md]] — track the close and next step.
- [[progress/review_ISS-32.1.md]], [[progress/review_history/ISS-32.1.md]], and [[progress/impl_ISS-32.1.md]] — retain gate and implementation records.
- [[progress/pr_ISS-32.md]] and [[progress/close_ISS-32.1.msg]] — provide the task narrative and harness commit message.
