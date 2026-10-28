# Impl 3.2.2 Set stopped

Files touched:
- coders/src/state/set_stopped.c (new, 42 header generated with Stdheader)
- coders/main.h (prototype in State section)
- coders/Makefile (SRC += src/state/set_stopped.c)
- tests/unit/test_set_stopped.c (new unit tests: success/idempotent/release, stop lock failure)

Design: lock log_mutex then stop_mutex; stop lock failure unlocks log_mutex and returns -1;
after stopped = 1 both unlocks (stop then log) always attempted, no short-circuit, -1 if either fails; prints nothing.

Results: make verify green (norminette OK, compile clean, tests passed; cppcheck not installed -> SKIP),
make check green (harness invariants OK), norminette coders/ no errors,
build/tests/acceptance/test_3.2.2.bin exits 0.
