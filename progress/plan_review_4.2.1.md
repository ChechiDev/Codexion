# Plan review — subtask 4.2.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `init_heap` is a core allocation function; its contract excludes threads, clocks, output, and global state. It supports the hand-written heap required by the subject.
- Scope: one public function and its type declaration and build entry, in three declared files. Allocation ownership and the failure guarantee are explicit; `free_heap` remains subtask 4.2.2.
- Test strategy: `tests/acceptance/test_4.2.1.c` has one black-box public-API test per AC. Success, capacity boundaries, invalid input, untouched output, independent allocations, and deliverable integration are covered. Allocation failure and implementation ordering are reserved for code review as the spec requires. The oracle is intentionally red until `t_heap` and `init_heap` exist. The harness supplied an outside-sandbox `make check` exit code of 0 before this review; no tests were run here.

## Acceptance tests (traceability)
- AC1 → `test_ac1_init_two_slots` (red)
- AC2 → `test_ac2_boundary_and_large_capacity` (red)
- AC3 → `test_ac3_invalid_capacity_preserves_heap` (red)
- AC4 → `test_ac4_null_heap` (red)
- AC5 → `test_ac5_independent_heaps` (red)
- AC6 → `test_ac6_build_symbol_and_norm` (red)
