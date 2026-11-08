# Implementation 9.2.3 Monitor reports failure

## Files touched
- coders/src/monitor/run_monitor.c (scan results now KEEP/SILENT/STOP/BURNED/ERROR; new `set_monitor_end(sim, result)`; `Updated:` header refreshed)
- tests/unit/test_run_monitor.c (asserts `sim.failed == 0` on the clean/burnout ends)

## Design
- Exits: silent (nothing), clean stop (`set_stopped` + `wake_dongles`, `set_failed` only if either fails), burnout (`wake_dongles`, `set_failed` on failure), error (`set_failed`, `set_stopped`, `wake_dongles`, each always tried).
- -1 from `get_burned_out`, `get_target_met`, `usleep` or `print_burnout` -> LEAVE_ERROR.
- 5 functions in file, each <= 25 lines, <= 4 params; no allocation/global/printf.

## Results
- `make verify`: norminette OK, compiles clean no relink, tests passed (cppcheck skipped, not installed).
- `make check`: exit 0, "Harness invariants OK" (log: build/check_9.2.3.log).
- Acceptance evidence (build/evidence_9.2.3.txt, dir build/tsan_9.2.3):
  - (a) `cc -Wall -Wextra -Werror -pthread -g -Icoders tests/acceptance/test_9.2.3.c $(coders/src/**/*.c)`: rc=0; `CODEXION_UNDER_VALGRIND=1 timeout 300 valgrind --trace-children=yes --error-exitcode=42 val_bin`: rc=0, 0 errors, no leaks in all processes.
  - (b) same plus `-fsanitize=thread`: build rc=0; `TSAN_OPTIONS=exitcode=66 timeout 300 setarch -R tsan_bin`: rc=0.
