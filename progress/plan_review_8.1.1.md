# Plan review — subtask 8.1.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `get_burned_out` belongs in monitor orchestration and uses the existing time adapter and the coder mutex. The spec keeps logging and stopping in the later monitor routine.
- Scope: one public function, with its source, declaration in `main.h`, and Makefile entry declared. The validated immutable configuration and output storage preconditions make the read-only contract coherent.
- Test strategy: seven black-box C cases exercise the public API through initialized simulation state. Deadline and extreme-value cases use a clamped clock; the real-clock case checks elapsed-time integration. The contention case checks the renewed value after releasing the coder mutex. The exact lock and clock ordering, failed synchronization paths, and overflow-safe arithmetic remain code-review checks as the spec states. The new function and declaration do not yet exist, so the oracle is red by design. The supplied pre-session harness check exited 0; no tests were run in this sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_deadline_boundary_and_no_side_effects` (red)
- AC2 → `test_ac2_other_duration` (red)
- AC3 → `test_ac3_real_clock` (red)
- AC4 → `test_ac4_satisfied_coder_still_burns` (red)
- AC5 → `test_ac5_extreme_starts` (red)
- AC6 → `test_ac6_contention_reads_renewed_start` (red)
- AC7 → `test_ac7_reject_null_inputs` (red)
