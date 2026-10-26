# Plan review — subtask 1.2.3

**Verdict:** PLAN_APPROVED

## Plan analysis
- **Layers:** Argument-count validation and usage output belong at the CLI entry point. `main.h` declares only the exit code and reusable usage function; the I/O header stays in `main.c`. This follows the subject's required argument order and allowed functions.
- **Scope:** The wrong-count path is one entry-point behavior. Its usage helper supports that path and the later parser. The three declared files cover the implementation, declaration, and required README update; dependency 1.2.2 is done.
- **Test strategy:** `tests/acceptance/test_1.2.3.c` checks the executable through its process exit status and captured streams, checks Norm, and compares the README example with the executable's actual usage output. It does not assert what eight arguments do. The supplied session-start `make check` exited 0. No tests were run in this sandbox; a syntax-only compile of the new acceptance file passed. The current stub `main` returns 0, so the wrong-count tests are expected to be red until implementation.

## Acceptance tests (traceability)
- AC1 → `test_ac1_no_arguments` (red: current stub exits 0 without usage)
- AC2 → `test_ac2_wrong_argument_counts` (red: current stub exits 0 without usage)
- AC3 → `test_ac3_usage_content` (red: current stub emits no usage)
- AC4 → `test_ac4_norminette_clean` (existing baseline is green)
- AC5 → `test_ac5_readme_instructions` (red: Instructions currently defer how to run the program)
