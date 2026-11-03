# impl 5.3.3 - free_coders

Files:
- coders/src/cleanup/free_coders.c (new, same shape as free_dongles.c)
- coders/main.h (prototype in Cleanup, Updated refreshed)
- coders/Makefile (SRC entry, Updated refreshed)
- tests/unit/test_free_coders.c (new unit test, written first)

Results (run sequentially):
- make check: exit 0, harness invariants OK
- make tsan: exit 0, "tsan clean"
- make memcheck: exit 0, "valgrind clean"
- norminette coders/: no non-OK lines
- valgrind on build/tests/acceptance/test_5.3.3.bin: exit 0, ERROR SUMMARY 0 errors
