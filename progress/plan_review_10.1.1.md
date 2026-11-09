# Plan review — subtask 10.1.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: the one-time seat offset belongs in coder orchestration and reuses the time adapter's `sleep_ms` contract. It does not change FIFO/EDF arbitration, shared state, or logging.
- Scope: one public function with three declared deliverable files: its source, the shared header, and the deliverable Makefile. The next subtask owns its integration into `run_coder`.
- Test strategy: seven black-box tests call only `sleep_start_turn` and existing public APIs. The test-defined clock stub exercises the stop and failure paths; the elapsed-time assertion has no upper bound. The oracle is red until the new function and header declarations exist. The supplied outside-sandbox `make check` exited 0; no tests were run here.

## Acceptance tests (traceability)
- AC1 → `test_ac1_odd_seat_no_sleep` (red)
- AC2 → `test_ac2_even_seat_waits` (red)
- AC3 → `test_ac3_even_seat_already_stopped` (red)
- AC4 → `test_ac4_invalid_pointers` (red)
- AC5 → `test_ac5_no_output_or_state_change` (red)
- AC6 → `test_ac6_stop_during_offset` (red)
- AC7 → `test_ac7_clock_failure` (red)
