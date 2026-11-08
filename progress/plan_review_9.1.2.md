# Plan review — subtask 9.1.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `set_failed` belongs in the state adapter. Its sole synchronization dependency is `stop_mutex`; it does not take the logging lock or assume responsibility for stopping or waking the run.
- Scope: one function, with its public declaration and build entry, is declared in exactly three production files. The dependency on completed 9.1.1 supplies the initialized `failed` field.
- Test strategy: eight black-box tests exercise the public C API and observable state, return codes, lock availability, concurrent use, and output. The error-checking mutex test is the specified deliberate exception to the initialized-simulation precondition. The unlock-error path is not safely inducible and remains a code-review check. The new tests are expected to be red until `set_failed` is implemented; no tests or builds were run in this sandbox. The supplied outside-sandbox baseline `make check` exited 0.

## Acceptance tests (traceability)
- AC1 → `test_ac1_marks_failure_without_stopping` (red)
- AC2 → `test_ac2_repeated_mark_is_idempotent` (red)
- AC3 → `test_ac3_marks_failure_after_stop` (red)
- AC4 → `test_ac4_releases_mutexes_after_success` (red)
- AC5 → `test_ac5_does_not_need_log_mutex` (red)
- AC6 → `test_ac6_lock_failure_preserves_state` (red)
- AC7 → `test_ac7_concurrent_writers_and_readers` (red)
- AC8 → `test_ac8_prints_nothing` (red)
