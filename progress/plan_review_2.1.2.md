# Plan review — subtask 2.1.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- **Layers:** `parse_number` is pure core parsing. It reuses `trim_span` and has no I/O, allocation, scheduler, or platform dependency. Field minimums remain with the later config parser.
- **Scope:** One function is the unit of work. The three declared files cover its source, public prototype, and deliverable build entry.
- **Test strategy:** Six black-box tests use the public API and observable build/Norm results. They cover accepted values, whitespace, invalid bytes, overflow, null pointers, output preservation, and input preservation. The oracle is intentionally red until implementation. The supplied session-start harness check exited 0; no tests were run in this sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_valid_numbers` (red)
- AC2 → `test_ac2_surrounding_whitespace` (red)
- AC3 → `test_ac3_invalid_digits` (red)
- AC4 → `test_ac4_overflow` (red)
- AC5 → `test_ac5_null_and_input_unchanged` (red)
- AC6 → `test_ac6_build_and_norm` (red)
