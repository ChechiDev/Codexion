# Implementation 4.2.2 Free heap

Files: coders/src/heap/free_heap.c (new), coders/main.h (prototype in Heap section, Updated refreshed), coders/Makefile (SRC entry, Updated refreshed), tests/unit/test_free_heap.c (new unit test).

Inner TDD: unit test written first; `make check` was red (exit 1, free_heap undefined). After implementation:

- make check: exit 0 (harness invariants OK, acceptance + unit green)
- make tsan: exit 0
- make memcheck: exit 0
- norminette coders/: no errors
- valgrind (full, error-exitcode=42) on build/tests/acceptance/test_4.2.2.bin: exit 0

Implementation: NULL returns first; one free(heap->items); items=NULL, size=0, capacity=0. No other allocation, lock, clock, output or global.
