# Implementation 6.2.2 get_deadline

Files touched:
- coders/src/time/get_deadline.c (new)
- coders/main.h (prototype in Time section, Updated: refreshed)
- coders/Makefile (SRC entry, Updated: refreshed)
- tests/unit/test_get_deadline.c (new, inner TDD)

Notes: validation first, one gettimeofday read, nsec carry computed in long,
overflow guard `now.tv_sec > LONG_MAX - secs` (time_t is long on the 64-bit
target), *out written only on success. No static helpers.

Results (run sequentially):
- make check: rc=0, "Harness invariants OK", compiles clean, sealed oracle intact.
- make tsan: rc=0, "[OK] tsan clean"
- make memcheck: rc=0, "[OK] valgrind clean"
- norminette coders/: all files OK (no error lines)
