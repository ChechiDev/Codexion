# Code review — subtask 3.4.4

**Verdict:** APPROVED

## Checkpoints
- C0: [x] `coders/src/log/print_burnout.c:43-54` commits one terminal stop and delegates the exact `burned out` line to the serialized logger; the supplied harness reports Norm green.
- C1: [x] The supplied `make check` exited 0 and reports the harness files and checks present.
- C2: [x] 3.4.4 is the sole in-progress leaf; the plan and feature index are synchronized, and its plan gate is approved.
- C3: [x] `print_burnout` is in the log adapter, uses the injected `t_sim`, and takes `log_mutex` before `stop_mutex` without globals or a new dependency.
- C4: [x] The three declared production files are the only production changes. `set_stopped_once` checks both mutex operations; `print_burnout` checks the log lock and unlock, and uses `print_log_line` once instead of duplicating timestamp, format, or write logic. No allocation or unbounded buffer operation was added.
- C5: [x] `tests/acceptance/test_3.4.4.c` has one public-API test for each AC1–AC12; the supplied green harness log reports the tests passed. Unit tests cover first stop, repeated call, invalid coder, and lock failure. No sanitizer result was supplied; that check is optional.
- C6: [x] Close documentation, done status, regenerated plan index, tracking, and task narrative are prepared; the harness will run the close check and make the single commit.

## Findings
- None.
