# Implementation 5.2.4 Init dongles

Files touched:
- coders/src/init/init_dongles.c (new)
- coders/main.h (prototype `init_dongles` in Init section; Updated refreshed)
- coders/Makefile (src/init/init_dongles.c added to SRC; Updated refreshed)
- tests/unit/test_init_dongles.c (new inner-TDD unit test)

Implementation follows the spec checklist: validation first (out NULL, n < 1), size bound
`(size_t)n > SIZE_MAX / sizeof(t_dongle)` before multiplying, one malloc, init_dongle in
order, rollback with `free_dongles(array, i)` then -1, `*out` written only on success.

Results (run sequentially):
- `make check`: all gates OK, "tests passed" (acceptance test_5.2.4 + unit green).
- `make tsan`: "[OK] tsan clean".
- `make memcheck`: "[OK] valgrind clean".
- `norminette coders/`: no errors.
- `valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42 build/tests/acceptance/test_5.2.4.bin`: exit 0.
