# impl ISS-46.2 (BLOCKED on test_9.2.3 AC2)
Files: coders/src/monitor/run_monitor.c (scan_step order stopped -> burnout -> target; scan-order comment). tests/unit/test_run_monitor.c does not pin the order: unchanged. Header Updated line untouched.
make verify: norminette OK, build OK, no relink OK; then FAIL:
  test_9.2.3.bin: test_9.2.3.c:174: test_ac2_zero_required_clean: Assertion `cap.len == 0' failed.
Cause: that sealed oracle pins target-before-burnout and is not in `migrates:`.
Manual run of every acceptance + unit test against the build: only test_9.2.3.c fails. ISS-46.2 and 8.2.1 pass.
TSan (setarch -R), ISS-46.2 and 8.2.1: rc 0, 0 warnings.
Valgrind ISS-46.2: rc 0. Valgrind 8.2.1: rc 134, test_8.2.1.c:202 assert_line `received_ms - start_ms <= 310 + SLACK` (AC1 non-first burnout timing; valgrind slowdown vs timing slack, reproducible 3/3, not ordering related; not compared against baseline).
