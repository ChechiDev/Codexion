# Impl 5.2.1 Init dongle

Files: coders/src/init/init_dongle.c (new), coders/main.h (t_dongle after t_heap, init_dongle prototype in Init, Updated refreshed), coders/Makefile (SRC + Updated), tests/unit/test_init_dongle.c (new unit test, written first).

init_dongle: NULL check; mutex -> cond -> init_heap(&heap, 2), NULL attrs; rollback in reverse on failure; available=1 and released_at=-1 only after all three succeed. No lock/clock/output/global.

Results (run sequentially):
- make check: all gates OK (acceptance 5.2.1 and unit tests green, integrity OK)
- make tsan: tsan clean
- make memcheck: valgrind clean
- norminette coders/: no errors
- valgrind (full, error-exitcode=42) on build/tests/acceptance/test_5.2.1.bin: 0 errors, no leaks, exit 0
