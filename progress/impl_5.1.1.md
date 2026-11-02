# Implementation 5.1.1 Get dongle pair

Files:
- coders/src/init/get_dongle_pair.c (new; validation first, right neighbour `seat == n - 1 ? 0 : seat + 1`, lower index into first, outputs written only on success)
- coders/main.h (new `/* ---- Init ---- */` section after Heap, prototype; Updated line refreshed)
- coders/Makefile (src/init/get_dongle_pair.c in SRC; Updated line refreshed)
- tests/unit/test_get_dongle_pair.c (new unit tests: ring pairs n=1..6, INT_MAX, invalid input)

Results (run sequentially):
- make check: green (norminette OK, tests passed incl. acceptance 5.1.1 and unit, harness invariants OK, 42 seals intact)
- make tsan: `[OK] tsan clean`
- make memcheck: `[OK] valgrind clean`
- norminette coders/: all files OK, no errors
