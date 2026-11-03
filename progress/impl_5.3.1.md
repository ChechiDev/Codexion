# Impl 5.3.1 Init coder

Files touched:
- coders/src/init/init_coder.c (created)
- coders/main.h (t_coder after t_dongle, init_coder prototype in Init, Updated refreshed)
- coders/Makefile (src/init/init_coder.c in SRC, Updated refreshed)
- tests/unit/test_init_coder.c (created: success seats, n == 1, invalid input byte-unchanged, NULL coder)

Implementation follows the code review check: validation (coder, sim NULL), then get_dongle_pair into locals,
then pthread_mutex_init; any failure returns -1 before other fields are written; id = seat + 1,
last_compile_start = 0, compile_count = 0, sim stored; thread never touched; 4 parameters.

Results (sequential runs):
- make check: rc 0 (harness invariants OK, all tests including unit test_init_coder and acceptance test_5.3.1 pass)
- make tsan: rc 0, "tsan clean"
- make memcheck: rc 0, "valgrind clean"
- norminette coders/: all files OK
- valgrind --leak-check=full ... build/tests/acceptance/test_5.3.1.bin: exit 0, 0 errors, no leaks
- build/tests/acceptance/test_5.3.1.bin and build/tests/unit/test_init_coder.bin: exit 0
