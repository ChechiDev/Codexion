# Plan review — subtask 2.2.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: moving `print_usage` from the entry point to `src/cli/` keeps user-facing output in the interfaces layer. The existing `main.h` declaration remains the public API until the later header-layout subtask.
- Scope: one function moves, and the three declared files cover its new translation unit, removal from `main.c`, and deliverable build registration. This is one atomic change with no new behavior or port required.
- Test strategy: `tests/acceptance/test_2.2.1.c` checks the executable and the public `print_usage()` API against the exact previously delivered output, then checks the deliverable build, no relink, and Norm. The test file passes a syntax-only compile. Tests were not run in this sandbox. AC2 is intentionally red until `print_usage()` is available from an object linked without `main.c`.
- Baseline: the supplied pre-session harness `make check` exited 0 (`/tmp/harness-check-plan.8iUR4q`).

## Acceptance tests (traceability)
- AC1 → `test_ac1_wrong_count_usage` (red at the plan gate through the new test binary's unresolved `print_usage` symbol).
- AC2 → `test_ac2_public_print_usage` (red: `print_usage` is currently defined only in `main.c`, which the acceptance test does not link).
- AC3 → `test_ac3_build_no_relink_and_norm` (red: the deliverable Makefile does not yet list `src/cli/print_usage.c`).
