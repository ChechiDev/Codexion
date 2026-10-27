# Plan review — subtask 2.2.7

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: removal of the obsolete `utils/` delegate and its public declaration stays within the pure core; the Makefile change removes its build entry. The remaining `trim_slice` and `parse_number` contracts are preserved.
- Scope: the three declared deliverable files form one atomic API retirement. `coders/main.h` and `coders/Makefile` are the project's required living files for this change. The deletion and build-list removal are also explicit code review checks.
- Test strategy: AC1 tests the public header by compiling separate translation units against each retired name and is expected to be red before implementation. AC2 and AC3 are regression checks. The oracle uses public APIs and deliverable commands; it does not inspect production source text.
- Older oracles: 2.1.1 AC1–AC6 and 2.2.2 AC2 are migrated to `trim_slice` and its Makefile entry without dropping their other assertions. A search also found an obsolete `trim_span` alternative in 2.2.3 AC2, which is migrated to the current `trim_slice` declaration while retaining that criterion's section checks. These three oracles are re-sealed as part of this gate.
- Baseline: the harness reported `make check` exit 0 before this gate (`/tmp/harness-check-plan.iOQWZe`). No tests were run in this sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_old_names_absent` (red until the old header names are removed)
- AC2 → `test_ac2_slice_and_number_regression` (regression)
- AC3 → `test_ac3_build_no_relink_and_norm` (regression)
