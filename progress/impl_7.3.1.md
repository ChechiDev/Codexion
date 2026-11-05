# Implementation 7.3.1 Run coder

## Files touched
- coders/src/coder/run_coder.c (new; 5 functions: set_failure, sleep_compile, sleep_rest, wait_single, run_coder)
- coders/main.h (prototype `void *run_coder(void *arg);`, Updated header line)
- coders/Makefile (src/coder/run_coder.c in SRC, Updated header line)
- tests/unit/test_run_coder.c (new unit test: invalid input, already stopped, compile_count overflow exit policy)

## Verification
- `norminette coders/`: no errors.
- `make check` (foreground, sequential): exit 0, all gates OK, acceptance 7.3.1 included.
  Acceptance binary build/tests/acceptance/test_7.3.1.bin (12:06:51) is newer than run_coder.c (11:53:35).
- valgrind on the acceptance binary (`--leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42 --trace-children=yes`): exit 0, "All heap blocks were freed", ERROR SUMMARY 0.
- TSan build (`cc -Wall -Wextra -Werror -pthread -g -fsanitize=thread`, run under `setarch -R`, `TSAN_OPTIONS=exitcode=66`): exit 0, 0 occurrences of "WARNING: ThreadSanitizer".

## Skipped
- `make tsan` / `make memcheck` targets not run separately (acceptance bin checked manually as requested).
- Unit test written alongside the code rather than strictly before it.
