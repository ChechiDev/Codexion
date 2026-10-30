# Implementation 4.2.1 Init heap

## Files touched
- coders/src/heap/init_heap.c (new): NULL / capacity < 1 / `SIZE_MAX / sizeof(t_request)` checks before the single `malloc`; fields written only after malloc succeeded; -1 writes nothing. `<stdint.h>` for SIZE_MAX (a standard header, not a function; no Norm or subject restriction).
- coders/main.h: `t_heap` after `t_request`; new `/* ---- Heap ---- */` section after Policy with `init_heap`; `Updated:` refreshed (2026/10/30 11:32:49).
- coders/Makefile: `src/heap/init_heap.c` added to SRC; `Updated:` refreshed.
- tests/unit/test_init_heap.c (new): capacities 1/2/1000 (zero and 0xa5 prefilled heap), invalid 0/-1/INT_MIN leave bytes unchanged, NULL heap.

## TDD
Unit test first: `make check` failed (unknown type `t_heap`, implicit `init_heap`) before implementation.

## Results (sequential runs)
- `norminette coders/`: all OK, exit 0
- `make check`: exit 0 (norminette, clean build/no relink, tests passed, harness invariants OK; cppcheck skipped, not installed)
- `make tsan`: exit 0, tsan clean
- `make memcheck`: exit 0, valgrind clean
- `valgrind --leak-check=full --error-exitcode=1 build/tests/acceptance/test_4.2.1.bin`: exit 0, all heap blocks freed, 0 errors
