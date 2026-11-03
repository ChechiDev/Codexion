# Impl 5.2.3 Free dongles
Files: coders/src/cleanup/free_dongles.c (new), coders/main.h (prototype, Updated), coders/Makefile (SRC, Updated), tests/unit/test_free_dongles.c (new).
Results (sequential): make check OK (harness invariants OK, no failures), make tsan "tsan clean", make memcheck "valgrind clean",
norminette coders/: 0 errors, valgrind on test_5.2.3.bin: 0 errors, no leaks, exit 0.
