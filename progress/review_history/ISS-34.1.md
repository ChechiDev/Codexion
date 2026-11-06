# Review history: ISS-34.1

## Round 1: plan gate, PLAN_APPROVED (2026-11-06 13:16)

### Plan review — subtask ISS-34.1

**Verdict:** PLAN_APPROVED

#### Plan analysis
- Layers and scope: the adapter change is confined to one declared production file, `coders/src/log/print_state.c`. It changes one public function's stop result and validates the coder before reading the stop flag. Existing caller behavior remains covered through public APIs.
- Test strategy: `tests/acceptance/test_ISS-34.1.c` has one black-box test per AC1–AC7. It exercises output, stop and failure return codes, concurrent calls, existing callers, and the deliverable. The test file passed a syntax-only compile; tests were not run in this sandbox. The supplied outside-sandbox baseline `make check` exited 0.
- Migrations: only the specified post-stop assertions and worker loop conditions changed in `test_3.4.3.c`, `test_3.4.4.c`, `test_3.4.5.c`, and `test_3.4.6.c`. Their failure assertions remain at -1. The acceptance oracle is intentionally red until the production contract changes.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_running_states` (red)
- AC2 → `test_ac2_stopped_states` (red)
- AC3 → `test_ac3_concurrent_stop` (red)
- AC4 → `test_ac4_failures_remain_errors` (existing failure behavior)
- AC5 → `test_ac5_rejected_input_wins` (red)
- AC6 → `test_ac6_callers_unchanged` (regression)
- AC7 → `test_ac7_build_symbol_norm` (regression)

## Round 2: plan gate, CHANGES_REQUESTED (2026-11-06 13:32)

### Plan review — subtask ISS-34.1

**Verdict:** CHANGES_REQUESTED

#### Plan analysis
- Layers and scope: the revised spec still confines the change to `coders/src/log/print_state.c`, one adapter function. Its return contract and caller regression checks are coherent with the existing public APIs.
- Test strategy: `tests/acceptance/test_ISS-34.1.c` retains one public API test per AC1–AC7. AC6(b) now checks the result, silence, and dongle availability without asserting that stopped requests are removed from heaps; the latter contradicted the sealed 6.3.4 contract. This plan re-review occurs after implementation, so the usual red-before-implementation state does not apply. No tests were run in this sandbox.
- Gate consistency: the supplied `make check` log reports that the ISS-34.1 spec changed after `ISSUE_APPROVED` and explicitly requires a new `issue ISS-34` review. The prior issue approval cannot authorize this revised spec at the plan gate.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_running_states`
- AC2 → `test_ac2_stopped_states`
- AC3 → `test_ac3_concurrent_stop`
- AC4 → `test_ac4_failures_remain_errors`
- AC5 → `test_ac5_rejected_input_wins`
- AC6 → `test_ac6_callers_unchanged` (corrected)
- AC7 → `test_ac7_build_symbol_norm`

#### Requested changes
1. Re-run the Codex issue gate for ISS-34 to review and seal the revised AC6(b) spec, then re-run this plan gate. The supplied check log also shows the old AC6(b) heap assertion failing at `tests/acceptance/test_ISS-34.1.c:449`; that assertion has been removed from the oracle.

## Round 3: plan gate, PLAN_APPROVED (2026-11-06 13:40)

### Plan review — subtask ISS-34.1

**Verdict:** PLAN_APPROVED

#### Plan analysis
- Layers and scope: one logging-adapter function in the sole declared production file, `coders/src/log/print_state.c`. The three-way result distinguishes a stopped simulation from an error while keeping callers' public behavior unchanged. The revised AC6(b) agrees with the sealed 6.3.4 stop contract: queued requests may remain in the heaps until `free_sim`.
- Test strategy: `tests/acceptance/test_ISS-34.1.c` has one public-API test for each AC1–AC7. AC6(b) checks the caller's return, silence, and dongle availability without asserting heap cleanup. The four listed migrated oracles change only stop-result expectations; rejected input and failure expectations remain -1. No tests were run in this sandbox.
- Gate state: `progress/issue_review_ISS-34.md` now records `ISSUE_APPROVED` for the revised spec. The supplied outside-sandbox `make check` exited 2 solely because the corrected ISS-34.1 oracle differs from its previous seal; the harness must reseal it after this plan approval. Production implementation already exists, so the usual pre-implementation red-test condition is inapplicable to this re-review.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_running_states`
- AC2 → `test_ac2_stopped_states`
- AC3 → `test_ac3_concurrent_stop`
- AC4 → `test_ac4_failures_remain_errors`
- AC5 → `test_ac5_rejected_input_wins`
- AC6 → `test_ac6_callers_unchanged` (corrected)
- AC7 → `test_ac7_build_symbol_norm`

## Round 1: code gate, PLAN_APPROVED (2026-11-06 13:45)

### Plan review — subtask ISS-34.1

**Verdict:** PLAN_APPROVED

#### Plan analysis
- Layers and scope: one logging-adapter function in the sole declared production file, `coders/src/log/print_state.c`. The three-way result distinguishes a stopped simulation from an error while keeping callers' public behavior unchanged. The revised AC6(b) agrees with the sealed 6.3.4 stop contract: queued requests may remain in the heaps until `free_sim`.
- Test strategy: `tests/acceptance/test_ISS-34.1.c` has one public-API test for each AC1–AC7. AC6(b) checks the caller's return, silence, and dongle availability without asserting heap cleanup. The four listed migrated oracles change only stop-result expectations; rejected input and failure expectations remain -1. No tests were run in this sandbox.
- Gate state: `progress/issue_review_ISS-34.md` now records `ISSUE_APPROVED` for the revised spec. The supplied outside-sandbox `make check` exited 2 solely because the corrected ISS-34.1 oracle differs from its previous seal; the harness must reseal it after this plan approval. Production implementation already exists, so the usual pre-implementation red-test condition is inapplicable to this re-review.

#### Acceptance tests (traceability)
- AC1 → `test_ac1_running_states`
- AC2 → `test_ac2_stopped_states`
- AC3 → `test_ac3_concurrent_stop`
- AC4 → `test_ac4_failures_remain_errors`
- AC5 → `test_ac5_rejected_input_wins`
- AC6 → `test_ac6_callers_unchanged` (corrected)
- AC7 → `test_ac7_build_symbol_norm`
