# Plan review — subtask 2.3.3

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `parse_config` stays in the pure parsing core. It reuses `parse_number`, `get_arg_minimum`, and `parse_mode`; no adapter, I/O, allocation, or global state is needed.
- Scope: one public function, one local helper, and three declared files (`parse_config.c`, `main.h`, `Makefile`). The helper only applies the existing numeric parser and minimum to one field.
- Test strategy: nine black-box checks call the public API through `main.h`, covering all fields, boundaries, independent and combined failures, and unchanged input. AC9 checks the deliverable build and Norm. The oracle is red until `parse_config` is declared and implemented. The supplied pre-session harness check exited 0; no tests were run in this review.

## Acceptance tests (traceability)
- AC1 → `test_ac1_valid_edf_config` (red)
- AC2 → `test_ac2_minimum_fifo_config` (red)
- AC3 → `test_ac3_trimmed_number` (red)
- AC4 → `test_ac4_positive_field_minimums` (red)
- AC5 → `test_ac5_invalid_numeric_positions` (red)
- AC6 → `test_ac6_invalid_scheduler` (red)
- AC7 → `test_ac7_all_invalid_positions_reported` (red)
- AC8 → `test_ac8_argv_unchanged` (red)
- AC9 → `test_ac9_build_and_norm` (red)
