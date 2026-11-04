# Implementation 7.1.2 Set compile count

## Files touched
- coders/src/coder/set_compile_count.c (new, 42 header)
- coders/main.h (prototype in Coder section, Updated line refreshed)
- coders/Makefile (SRC entry, Updated line refreshed)
- tests/unit/test_set_compile_count.c (new, inner TDD: increment, NULL, bounds INT_MAX / negative, mutex unlocked)

## Design
NULL check before any lock; lock coder mutex; reject compile_count < 0 or >= INT_MAX (status -1, field unchanged); else increment; always unlock (unlock failure -> -1, no rollback). Only the coder's own mutex is taken.

## Verification (real results)
- `make check` (foreground, sequential): exit 0, harness invariants OK, 63 seals intact, src scope OK.
- Acceptance binary build/tests/acceptance/test_7.1.2.bin (19:01:39) is newer than set_compile_count.c (18:51:58).
- `norminette coders/`: no non-OK lines.
- valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42 on the acceptance bin: exit 0, 0 errors, no leaks.
- Acceptance built with `-fsanitize=thread` and run under `setarch -R`: exit 0, 0 "WARNING: ThreadSanitizer".
- Unit test under valgrind: clean.
- Skipped: `make tsan` / `make memcheck` full targets were not run separately (manual TSan/valgrind runs above instead).
