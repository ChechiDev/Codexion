# Implementation 3.1.2 Get elapsed ms

Files touched:
- coders/src/time/get_elapsed_ms.c (new, official 42 header): get_time_ms, -1 checked first, clamp to 0 if now < start_ms.
- coders/main.h: prototype in Time section.
- coders/Makefile: src/time/get_elapsed_ms.c added to SRC.
- tests/unit/test_get_elapsed_ms.c (new, unit tests).

Results:
- make verify: norminette OK, compiles clean, tests passed (cppcheck skipped, not installed).
- make check: exit 0, harness invariants OK.
- norminette coders/: no errors.
- build/tests/acceptance/test_3.1.2.bin run directly: exit 0.
