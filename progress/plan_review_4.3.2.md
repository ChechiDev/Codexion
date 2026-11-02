# Plan review — subtask 4.3.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `get_head_request` is a pure core heap read. The caller owns synchronization; no adapter dependency or new port is needed.
- Scope: one public function and three declared production files cover the implementation, prototype, and build entry. Dependency 4.3.1 is done, and phase 04 is approved.
- Test strategy: `tests/acceptance/test_4.3.2.c` uses the public heap API and checks the returned value, unchanged heap fields and slots, failure outputs, and copy semantics. The build, exported symbol, and Norm are checked separately. The oracle is intentionally red until implementation. The supplied outside-sandbox `make check` exited 0 before this oracle was written; no tests were run in this review.

## Acceptance tests (traceability)
- AC1 → `test_ac1_single_head` (red)
- AC2 → `test_ac2_successive_minimums` (red)
- AC3 → `test_ac3_read_only` (red)
- AC4 → `test_ac4_empty_unchanged` (red)
- AC5 → `test_ac5_null_arguments` (red)
- AC6 → `test_ac6_copy_by_value` (red)
- AC7 → `test_ac7_build_symbol_and_norm` (red)
