# Implementation 3.4.4 print_burnout

Files: coders/src/log/print_burnout.c (new, 42 header via Stdheader), coders/main.h (prototype in Log section, Updated refreshed), coders/Makefile (SRC + Updated refreshed), tests/unit/test_print_burnout.c (new).

Design: claim_stop (lock stop_mutex, check/set stopped, unlock) and write_line (clock, build_log_line, one write) static helpers; print_burnout locks log_mutex, claims the stop, writes, unlocks log_mutex. set_stopped is not called. Every mutex result checked.

Results: make verify green (norminette OK, clean build, unit tests pass); norminette coders/ clean; make check green (harness invariants OK); build/tests/acceptance/test_3.4.4.bin rc=0, also with CODEXION_TIME_SLACK_MS=40.
