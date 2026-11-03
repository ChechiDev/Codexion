# impl 5.4.1 Free sim

Files touched:
- coders/src/cleanup/free_sim.c (new; static helper `free_arrays` + `free_sim`)
- coders/main.h (t_sim moved after t_coder with the spec fields/order; free_sim prototype; Updated refreshed)
- coders/Makefile (free_sim.c in SRC; Updated refreshed)
- tests/unit/test_free_sim.c (new unit test, written first)

Results (run sequentially):
- First `make check`: norminette TOO_MANY_LINES on free_sim (43 lines), fixed by extracting `free_arrays`. A test_2.2.3 failure was seen in that same run (not reproduced).
- `make check`: rc=0, all gates green (acceptance incl. 5.4.1, unit, lint, integrity).
- `make tsan`: tsan clean.
- `make memcheck`: valgrind clean.
- `norminette coders/`: no errors.
- valgrind on build/tests/acceptance/test_5.4.1.bin: exit 0, all heap blocks freed, 0 errors.
