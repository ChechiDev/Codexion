---
id: ISS-46.2
alias:
  - Burnout before target
  - Monitor burnout precedence
tags:
  - codexion
  - monitor
  - burnout
  - simulation
date: 2026-11-12 17:17
---

<!-- section: concept -->
## Reporting burnout when the target is also met

The monitor observes shared simulation state in periodic scans. A scan can find both that a coder's deadline has passed and that every coder has reached the compile target. Since the state records the deadline and compile counts but not the precise order in which those events happened, the monitor needs a defined tie-break.

The chosen policy is conservative: preserve an externally requested stop first, then report an observed burnout, and only then stop cleanly for the target.

```text
scan begins
   │
   ├─ already stopped? ── yes → leave silently
   │
   ├─ burnout found? ──── yes → report burnout, stop and wake waiters
   │
   ├─ target met? ─────── yes → stop and wake waiters
   │
   └─ otherwise → wait about 1 ms and scan again
```

This order does not reconstruct exact event chronology; it ensures that a reached deadline is not hidden by a target check in the same observation. The zero-required application case remains silent because `run_simulation` returns before creating the monitor.

<!-- section: planning -->
## How it was planned

ISS-46 divides the missed-burnout defect into preserving a deadline when a coder attempts to start compiling after it expires (ISS-46.1) and choosing burnout precedence in the monitor (ISS-46.2). This subtask declared only `coders/src/monitor/run_monitor.c` for production changes. Its six acceptance criteria cover joint observation, target-only and burnout-only states, an external stop, the zero-required binary behavior, and build, symbol, and Norm requirements. The sealed 8.2.1 and 9.2.3 monitor oracles were listed for migration where they asserted target-first behavior.

<!-- section: execution -->
## How it was executed

`scan_step` keeps the existing external-stop check and burnout selection, logging, stop, and wake-up helpers. It checks for burnout before checking the compile target. This keeps the change in monitor orchestration and leaves error handling and waiter wake-ups on their existing paths. The acceptance oracle exercises the monitor directly with controlled shared state; the two historical oracles were updated only at cases that encoded the superseded precedence.

<!-- section: problems -->
## Problems & resolution

The initial migration list omitted `test_9.2.3.c` AC2, which also asserted silent target-first behavior for a synthetic zero-required monitor state. The failure exposed the omitted sealed oracle. The spec was corrected to include that migration, and the final issue review confirmed all three affected monitor-oracle locations. Re-sealing then encountered a gate cycle because the earlier subtask was already done while the issue lacked its approval verdict; the issue gate recorded the check failure and subsequently approved using the harness's counterfactual check that assumed only the issue verdict. Code review also requested refreshing the legacy 42 header's `Updated:` field; that was done before approval. The Valgrind run of migrated 8.2.1 has a baseline-reproduced timing assertion failure, documented in the implementation report and review; it is unrelated to memory errors or this change.

<!-- section: files -->
## Files created / modified

- [[coders/src/monitor/run_monitor.c]] — checks stopped state, then burnout, then compile target.
- [[tests/acceptance/test_ISS-46.2.c]] — covers the six approved criteria for the monitor change.
- [[tests/acceptance/test_8.2.1.c]] — migrates the target-first burnout cases to burnout-first expectations.
- [[tests/acceptance/test_9.2.3.c]] — migrates AC2's synthetic zero-required monitor expectation.
- [[docs/plan/issues/ISS-46-fix-an-expired-burnout-deadline-can-be-erased-before-the-monitor-sees-it-8-1/ISS-46.2-burnout-before-target.md]] — marks the approved subtask complete.
- [[docs/plan/PLAN.md]] — regenerated issue index.
- [[feature_list.json]] — regenerated plan index.
- [[progress/current.md]] — records the close and task completion.
- [[progress/history.md]] — appends the close entry.
- [[progress/pr_ISS-46.md]] — task PR narrative.
- [[progress/close_ISS-46.2.msg]] — commit message for the harness close.
