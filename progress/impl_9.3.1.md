# Implementation 9.3.1 Join threads

## Files touched
- coders/src/lifecycle/join_threads.c (new, 42 header, 1 function)
- coders/main.h (Lifecycle section with `join_threads` prototype, Updated line refreshed)
- coders/Makefile (SRC += src/lifecycle/join_threads.c, Updated line refreshed)
- tests/unit/test_join_threads.c (new inner unit test: NULL, -1, > count, partial join of 2 coders + monitor)

## make verify (real result: FAILS, unrelated to this subtask)
```
[OK]    norminette
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_1.1.1.bin: tests/acceptance/test_1.1.1.c:26: capture: Assertion `feof(pipe)' failed.
[FAIL]  tests/acceptance/test_1.1.1.c
```
Cause: sealed oracle test_1.1.1 captures `make -C coders re` into `char output[8192]`
(line 204-207). That output is now 8344 bytes (was just under 8192 before; every new
SRC entry adds about 135 bytes: compile line + rm line + link line), so `feof` fails.
It is a buffer-size defect of the oracle triggered by adding any file to SRC, not a
behaviour failure. It will hit every future subtask until test_1.1.1 is fixed through the
plan gate. All other tests, run one by one with the same flags as `make test`
(all tests/unit/*, all tests/acceptance/* including test_9.3.1 and test_join_threads),
pass; only test_1.1.1 fails (rc=134).

## Acceptance evidence (build/tsan_9.3.1)
(a) `cc -Wall -Wextra -Werror -pthread -g -Icoders tests/acceptance/test_9.3.1.c <all coders/src/**/*.c> -o build/tsan_9.3.1/plain`
build_rc=0; `CODEXION_UNDER_VALGRIND=1 timeout 300 valgrind --trace-children=yes --error-exitcode=42 build/tsan_9.3.1/plain`
-> ERROR SUMMARY: 0 errors, all heap blocks freed, valgrind_rc=0.

(b) same plus `-fsanitize=thread` -> tsan_build_rc=0;
`TSAN_OPTIONS=exitcode=66 CODEXION_UNDER_VALGRIND=1 timeout 120 setarch -R build/tsan_9.3.1/tsan` -> tsan_rc=0.
