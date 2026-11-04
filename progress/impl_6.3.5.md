# Implementation 6.3.5 Take single dongle

Files touched:
- coders/src/dongle/take_single_dongle.c (new, 5 functions: get_single_dongle, push_single_request, wait_for_single_dongle, take_single_step, take_single_dongle)
- coders/main.h (prototype, Updated: refreshed)
- coders/Makefile (SRC entry, Updated: refreshed)
- tests/unit/test_take_single_dongle.c (new)

Real results:
- make check: all sections [OK] (tests passed, norminette OK, compiles clean, seals intact, src scope OK).
- acceptance test_6.3.5.bin (built 15:24, after last source edit 15:20): exit 0.
- valgrind --leak-check=full ... --error-exitcode=42 on the acceptance binary: ERROR SUMMARY 0 errors, exit 0.
- TSan (setarch -R, acceptance source + coders/src/*/*.c): no warnings, exit 0.

Rework after Codex CHANGES_REQUESTED: push_single_request now checks the unlock on the failed-push path and returns -2 (fatal); take_single_dongle skips the second unlock for -2 and runs set_stopped + wake_dongles, returning -1.
Rerun: make check green (norminette OK, tests passed, invariants OK); acceptance bin (15:35) newer than source (15:33), exit 0; valgrind 0 errors exit 0; TSan under setarch -R clean exit 0.
