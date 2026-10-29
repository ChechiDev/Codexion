# Plan review — subtask 4.1.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: the comparator belongs to pure core policy code. The spec excludes I/O, clocks, locks, allocation, and global state; the public `main.h` API supports black-box tests.
- Scope: one function, `is_request_before`, with three declared production files: its source, the public header, and the deliverable Makefile. The request type and prototype support that function. Later heap and arbitration work stays in later subtasks.
- Test strategy: nine acceptance tests exercise the public comparator, its value and NULL boundaries, strict lexicographic order, coder independence, and preservation of caller data. The deliverable build, exported symbol, and Norm are checked separately. The oracle is intentionally red until the type and function are implemented.
- Gate: the supplied outside-sandbox `make check` exited 0 before this plan review. No tests were run in the sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_smaller_key_first` (red)
- AC2 → `test_ac2_ticket_breaks_key_tie` (red)
- AC3 → `test_ac3_strict_order` (red)
- AC4 → `test_ac4_coder_does_not_affect_order` (red)
- AC5 → `test_ac5_extreme_values` (red)
- AC6 → `test_ac6_sample_total_order` (red)
- AC7 → `test_ac7_null_arguments` (red)
- AC8 → `test_ac8_arguments_read_only` (red)
- AC9 → `test_ac9_build_symbol_and_norm` (red)
