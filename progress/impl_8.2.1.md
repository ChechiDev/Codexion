# Implementation report 8.2.1 Run monitor

## Files touched
- coders/src/monitor/run_monitor.c (new, 42 header, 5 functions: get_burned_index, handle_burnout, scan_step, run_monitor + 4 LEAVE/KEEP defines)
- coders/main.h (prototype `void *run_monitor(void *arg);` in Monitor section, Updated refreshed)
- coders/Makefile (src/monitor/run_monitor.c in SRC, Updated refreshed)
- tests/unit/test_run_monitor.c (new inner-TDD unit test: NULL, target met, zero target, burnout)

## Design
Scan order: is_stopped (leave silently) -> get_target_met (stop + wake) -> get_burned_index
(single static helper, ascending scan, lowest index) -> print_burnout at once -> usleep(1000).
Failures: set_stopped then wake_dongles (wake always attempted). Burnout success: wake only
(print_burnout already stopped). No mutex taken directly, no wake while holding one, NULL on all paths.

## Verification (real results)
- `make check`: all sections OK (build clean, no relink, unit + acceptance green, norminette OK, integrity OK; only the pre-existing WARN about .harness/templates.json).
- `norminette coders/`: every file OK, no error.
- Acceptance test_8.2.1 with -DCODEXION_TIME_SLACK_MS=40 under valgrind (leak-check full, error-exitcode=42): rc 0, "All heap blocks were freed", 0 errors.
- Acceptance test_8.2.1 and unit test with -fsanitize=thread (fresh objects) under `setarch -R`: rc 0, no ThreadSanitizer warning.
- Plain acceptance build: rc 0.
