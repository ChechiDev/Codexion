# Implementation 3.1.1 Get time ms

Files touched:
- coders/src/time/get_time_ms.c (new, 42 header via Stdheader on scratch copy; includes stddef.h, sys/time.h, main.h; returns -1 if gettimeofday fails)
- coders/main.h (new `/* ---- Time ---- */` section between Parsing and CLI with `long get_time_ms(void);`)
- coders/Makefile (src/time/get_time_ms.c added to SRC)
- tests/unit/test_get_time_ms.c (new unit tests: epoch scale, non-decreasing)

Results:
- make verify: norminette OK, deliverable compiles clean, all tests passed.
- make check: all sections OK, "Harness invariants OK".
- norminette coders/: no errors.
- build/tests/acceptance/test_3.1.1.bin run directly: rc=0.
