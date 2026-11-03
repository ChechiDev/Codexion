# Impl 5.3.4 init_coders

Files: coders/src/init/init_coders.c (new), coders/main.h (prototype, Updated), coders/Makefile (SRC, Updated), tests/unit/test_init_coders.c (new).

Results (run sequentially):
- make check: harness invariants OK, all green (acceptance 5.3.4 + unit)
- make tsan: tsan clean
- make memcheck: valgrind clean
- norminette coders/: no errors
- valgrind on build/tests/acceptance/test_5.3.4.bin: 0 errors, all heap blocks freed
