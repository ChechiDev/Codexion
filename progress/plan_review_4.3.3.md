# Plan review — subtask 4.3.3

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `pop_request` is a core heap operation. It uses the existing public request order; locking stays with the later dongle caller.
- Scope: one public function with a local sift helper. The three declared files cover its implementation, public declaration, and deliverable build entry. Dependency 4.3.2 is done, and phase 04 is approved.
- Test strategy: `tests/acceptance/test_4.3.3.c` checks the public API, heap contents, failure preservation, ordering at small and large capacities, interleaving, and the deliverable build and Norm. Each AC has exactly one named test. The oracle is intentionally red until `pop_request` is implemented. The supplied outside-sandbox `make check` exited 0 before the oracle was written; no tests were run here.

## Acceptance tests (traceability)
- AC1 → `test_ac1_pop_single_request` (red)
- AC2 → `test_ac2_sorted_order` (red)
- AC3 → `test_ac3_heap_property_and_content` (red)
- AC4 → `test_ac4_empty_unchanged` (red)
- AC5 → `test_ac5_extremes_and_large_heap` (red)
- AC6 → `test_ac6_interleaved` (red)
- AC7 → `test_ac7_dongle_sized_heap` (red)
- AC8 → `test_ac8_invalid_arguments` (red)
- AC9 → `test_ac9_build_symbol_and_norm` (red)
