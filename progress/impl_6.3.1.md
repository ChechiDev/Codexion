# Impl 6.3.1 Wake dongles

Files: coders/src/dongle/wake_dongles.c (new), coders/main.h (prototype in Dongle section, Updated line refreshed), coders/Makefile (SRC entry, Updated line refreshed), tests/unit/test_wake_dongles.c (new unit test).

Results:
- `make check`: rc=0 (norminette OK, clean build -Wall -Wextra -Werror, tests passed, harness invariants OK).
- valgrind (leak-check=full, error-exitcode=42) on build/tests/acceptance/test_6.3.1.bin: exit 0, 0 errors, no leaks.
- TSan build of test_6.3.1.c + coders/src/*/*.c under `setarch -R`: exit 0, 0 "WARNING: ThreadSanitizer".
- Note: cppcheck not installed (skipped by make check).
