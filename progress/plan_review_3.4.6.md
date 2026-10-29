# Plan review — subtask 3.4.6

**Verdict:** PLAN_APPROVED

## Plan analysis

- Layers: `print_log_line` is an adapter for clock access and stdout; its caller owns serialization and stop policy. `print_state` delegates the shared output sequence while keeping its existing lock order and stop check.
- Scope: one new public function and its integration into the existing caller, with three declared files. The `Makefile` entry is necessary to build the new translation unit. The prior 3.4.5 declaration supplies the public contract.
- Test strategy: eight black-box acceptance checks call the declared API or inspect the built binary. AC7 checks `print_state` behavior directly; the sealed 3.4.3 oracle remains part of the full test suite. The new oracle is expected to be red until implementation. The harness reported `make check` exit 0 before this plan gate; no tests were run in this sandbox. The oracle passed a C syntax-only compile.

## Acceptance tests (traceability)

- AC1 → `test_ac1_line_and_lock_ownership` (red)
- AC2 → `test_ac2_timestamp_bounds` (red)
- AC3 → `test_ac3_unbuffered_pipe_visibility` (red)
- AC4 → `test_ac4_no_lock_or_stop_check` (red)
- AC5 → `test_ac5_invalid_line_rejected` (red)
- AC6 → `test_ac6_write_error` (red)
- AC7 → `test_ac7_print_state_regression` (red)
- AC8 → `test_ac8_build_symbols_and_norm` (red)
