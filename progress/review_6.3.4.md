# Code review — subtask 6.3.4

**Verdict:** APPROVED

## Checkpoints

- C0: [x] `coders/src/dongle/take_dongles.c:46-98` uses the authorized synchronization and heap APIs, enforces head-of-both acquisition and cooldown, and adds no global, output, or allocation. The supplied check reports Norm clean.
- C1: [x] The supplied outside-sandbox `make check` exited 0; its log confirms the build, tests, plan sync, gate consistency, and harness integrity. `make status` identifies 6.3.4 as the only in-progress leaf.
- C2: [x] `progress/current.md:3` accurately records the completed helper rename and corrected-oracle TSan run. The plan review is `PLAN_APPROVED`, and the supplied check confirms the generated plan index and completed gates are consistent.
- C3: [x] `coders/src/dongle/take_dongles.c:13-124` keeps arbitration in `dongle/`, reuses the request, readiness, heap, time, stop, and wait APIs, and adds no global state or circular dependency. `coders/main.h:188` exposes the function in the project header.
- C4: [x] `coders/src/dongle/take_dongles.c:15-26` validates inputs before mutation; lines 41-68 handle request and lock failures; lines 70-124 release locks and shut down on queued failures. The function is decomposed into four short static helpers and adds no unsafe buffer or sensitive output. The production changes match the three declared files (`coders/src/dongle/take_dongles.c`, `coders/main.h`, `coders/Makefile`).
- C5: [x] `tests/acceptance/test_6.3.4.c:142-385` contains one public-API test for each AC1–AC7, and the supplied check passes. `tests/unit/test_take_dongles.c:20-59` covers acquisition, stop, and invalid input. `progress/impl_6.3.4.md` records Valgrind and corrected-oracle TSan exit 0.
- C6: [ ] Close-gate work remains: reference documentation, `done` status, history entry, and the single commit. This checkpoint does not apply to code approval.

## Findings

No blocking findings. The previous state-summary issue is resolved at `progress/current.md:3`.
