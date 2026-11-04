# Impl 6.3.2 Wait for dongles

Files: coders/src/dongle/wait_for_dongles.c (new), coders/main.h (prototype, Updated header),
coders/Makefile (SRC entry, Updated header), tests/unit/test_wait_for_dongles.c (unit).

Design: validation first (-1, no lock); get_deadline(1) while both held; unlock second;
pthread_cond_timedwait on first cond+mutex; relock second; ETIMEDOUT and wake-up -> 0;
any runtime failure releases what is held (release_pair) and returns -1.

Real output:
- make check: norminette OK, build clean (-Wall -Wextra -Werror), tests passed,
  harness invariants OK (src scope, seals intact).
- valgrind (leak-check=full, show-leak-kinds=all, errors-for-leak-kinds=all, error-exitcode=42)
  on test_6.3.2.bin: 29 allocs, 29 frees, ERROR SUMMARY 0 errors, rc 0.
- TSan (-fsanitize=thread, setarch -R) on the 6.3.2 acceptance: no warning, rc 0.
- Unit test test_wait_for_dongles built with -Werror: rc 0.

Revision after Codex CHANGES_REQUESTED: release_pair now attempts to unlock the second mutex when the second unlock itself failed (hold_second=1), then the first; cleanup results are explicitly discarded with (void) since nothing more can be done. Re-run: make check rc=0 (norminette OK, tests passed, invariants OK); valgrind 0 errors rc 0; TSan under setarch -R clean rc 0; unit rc 0. Failure paths are not unit-testable without mutex fault injection.
