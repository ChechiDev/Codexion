# Plan review — subtask 5.4.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: simulation resource setup belongs in the `init/` adapter and reuses the existing dongle, coder, and teardown APIs. No core or interface dependency is added.
- Scope: one public initializer, `init_simulation`, with its source, `main.h` declaration, and deliverable Makefile entry declared. Dependency 5.4.1 is done. The phase 05 re-review approved the name change.
- Test strategy: six public-API tests map one-to-one to AC1–AC6. They cover initialized fields and usable mutexes, boundary sizes, stop-state integration, untouched invalid output, and reuse after teardown. Allocation and pthread initialization failures cannot be forced deterministically by this oracle; the specified rollback is a code-review check.
- Supplied check: `make check` exited 2 because the prior oracle still called `init_sim`. This plan gate changes those calls to the approved `init_simulation` name. The oracle was originally written before implementation; an implementation is now present from the interrupted attempt, so its current red/green status is unverified. No tests were run in this sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_initialize_four_coders` (status unverified after rename)
- AC2 → `test_ac2_initialize_boundary_counts` (status unverified after rename)
- AC3 → `test_ac3_stop_state_api` (status unverified after rename)
- AC4 → `test_ac4_initialize_sentinel_buffer` (status unverified after rename)
- AC5 → `test_ac5_reject_invalid_input_unchanged` (status unverified after rename)
- AC6 → `test_ac6_reinitialize_after_free` (status unverified after rename)
