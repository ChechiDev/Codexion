# Implementation 6.3.3 Release dongles

Files touched:
- coders/src/dongle/release_dongles.c (new)
- coders/main.h (prototype in Dongle section; Updated: refreshed)
- coders/Makefile (SRC entry; Updated: refreshed)
- tests/unit/test_release_dongles.c (new)

Notes: dongles are locked lower index first, the second only if different; a failed second lock unlocks the first and returns -1; the clock is read after both locks (failure unlocks both, nothing changed); both dongles updated then broadcast; both mutexes always unlocked (reverse order) with broadcast/unlock failures folded into -1 without skipping steps. Extra guard: dongle indices outside [0, number_of_coders) return -1. get_elapsed_ms clamps to 0, so a clock failure cannot be provoked in a unit test (reviewed only).

Results:
- make check: rc=0 (norminette OK, clean build, unit tests passed, harness invariants OK)
- valgrind (full, error-exitcode=42) on build/tests/acceptance/test_6.3.3.bin: exit 0, no leaks, 0 errors
- TSan build of test_6.3.3.c under setarch -R: exit 0, no WARNING
- valgrind on the unit binary: exit 0

Rework (Codex CHANGES_REQUESTED): mark_released now writes the second dongle only when high != low (single dongle updated once). make check rc=0, valgrind on acceptance bin rc=0, TSan under setarch -R rc=0 with no warning.
