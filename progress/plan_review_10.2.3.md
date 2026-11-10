# Plan review — subtask 10.2.3

**Verdict:** PLAN_APPROVED

## Plan analysis
- The change is confined to three declared CLI interface files. It applies one narrow rule to the remaining fixed-text error output while preserving the formatted numeric errors, public signatures, and message bytes. No core or adapter dependency changes.
- The acceptance oracle uses only the public functions and delivered binary. It checks exact byte counts and content on both streams, the closed-stderr exit path, and the unresolved symbol list. AC1 is expected to be red against the current binary's `fwrite` reference; the message regression checks may already pass. No tests were run in this sandbox; the C file passed a warning-clean syntax check.

## Acceptance tests (traceability)
- AC1 → `test_ac1_no_forbidden_symbols` (red)
- AC2 → `test_ac2_usage_exact_stderr` (existing behaviour)
- AC3 → `test_ac3_run_error_exact_stderr` (existing behaviour)
- AC4 → `test_ac4_argument_errors` (existing behaviour)
- AC5 → `test_ac5_closed_stderr_exit` (existing behaviour)
