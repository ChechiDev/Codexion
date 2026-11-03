# Plan review — subtask 5.2.4

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: array allocation and pthread resource lifecycle belong in the init adapter; the plan reuses the existing `init_dongle` and `free_dongles` APIs.
- Scope: one `init_dongles` function with three declared production files (source, public prototype, and build list). Dependencies 5.2.1 and 5.2.3 are complete. The output pointer is assigned only after full success, and partial initialization uses the established cleanup path.
- Test strategy: four black-box C tests exercise the public API and observable state, including the one- and 200-dongle sizes, element independence, and invalid arguments. Allocation and pthread failure rollback remain explicit code-review checks because this harness has no portable deterministic fault injection. The oracle is expected to be red until the new function is implemented; tests were not run in this sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_four_initialized_dongles` (red)
- AC2 → `test_ac2_one_and_two_hundred_dongles` (red)
- AC3 → `test_ac3_independent_elements` (red)
- AC4 → `test_ac4_invalid_arguments_preserve_output` (red)
