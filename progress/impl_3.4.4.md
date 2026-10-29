# Implementation 3.4.4 Print burnout (replanned implementation)

Written fresh (nothing restored from the stash), on top of 3.4.5/3.4.6.

Files:
- coders/src/log/print_burnout.c (new, 42 header via Stdheader): `print_burnout` plus static `set_stopped_once`.
- coders/main.h: `print_burnout` prototype in the Log section; `Updated:` refreshed.
- coders/Makefile: `src/log/print_burnout.c` added to SRC; `Updated:` refreshed.
- tests/unit/test_print_burnout.c (new unit tests).

Design: lock log_mutex; `set_stopped_once` locks stop_mutex, sets stopped=1 if it was 0, unlocks stop_mutex (0 only if it made the first stop); then `print_log_line(sim, coder, "burned out")` once under log_mutex; unlock log_mutex; return 0 only if all steps succeeded. No set_stopped call, no own timestamp/build/write.

Results: `make verify` green (norminette OK, build clean, tests passed), `make check` green (harness invariants OK), `norminette coders/` clean, build/tests/acceptance/test_3.4.4.bin passes (rc 0), also with CODEXION_TIME_SLACK_MS=40.
