# Plan review — subtask 2.1.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- **Layers:** `trim_span` is a pure core function with no I/O, allocation, or platform dependency. The public `t_span` type and prototype belong in `coders/main.h` for this deliverable.
- **Scope:** One span-trimming function is the unit of work. The three declared files cover the source, public header, and deliverable build. The static whitespace helper serves that function and does not add a separate concern.
- **Test strategy:** The six black-box acceptance tests exercise the public API and observable build/Norm outcomes. The function and type do not yet exist, so the oracle is intentionally red. The supplied session-start harness check exited 0; no tests were run in this sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_basic_span` (red)
- AC2 → `test_ac2_six_whitespace_bytes` (red)
- AC3 → `test_ac3_inner_whitespace_preserved` (red)
- AC4 → `test_ac4_empty_and_null` (red)
- AC5 → `test_ac5_input_unchanged` (red)
- AC6 → `test_ac6_build_and_norm` (red)
