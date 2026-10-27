# Plan review — subtask 2.2.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `trim_span` is pure, generic string handling. Moving it from parsing to utils preserves the core dependency direction; no port or adapter is needed.
- Scope: this is one function move. The three declared files cover its old and new source paths and the deliverable Makefile. Deferring the `main.h` section change to 2.2.3 keeps this subtask atomic.
- Test strategy: AC1 checks trimming through the public `parse_number` API, including all six surrounding whitespace bytes and an invalid inner space. AC2 checks the deliverable build, source registration, no relink, and Norm. The new oracle is red against the current Makefile, which still lists `src/parsing/trim_span.c`. Both acceptance files pass a syntax-only compile; tests were not run in this sandbox.
- Older oracle compatibility: only the AC6 Makefile path in `test_2.1.1.c` changed to `src/utils/trim_span.c`; its SHA-256 manifest was updated. The other five checks and the remainder of AC6 are unchanged.
- Baseline: the supplied pre-session harness `make check` exited 0 (`/tmp/harness-check-plan.Hn9bJk`).

## Acceptance tests (traceability)
- AC1 → `test_ac1_trimmed_numbers` (behavior already delivered; remains valid after the move).
- AC2 → `test_ac2_build_no_relink_and_norm` (red until the Makefile registers the moved source).
