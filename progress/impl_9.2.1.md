# Implementation 9.2.1 - Take keeps print result

## Files touched
- coders/src/coder/take_coder_dongles.c: `print_taken` returns the first non-zero `print_state` result (1 or -1); `release_taken(coder, taken, print_result)` releases once, returns -1 if release failed or print_result < 0, else `*taken = 0` and 0; no more `is_stopped` re-read; header `Updated:` refreshed.
- tests/unit/test_take_coder_dongles.c: added `check_print_failure` (/dev/full on stdout: -1, taken unchanged, dongles restored, not stopped).

## make verify
Exit 0. Tail: `[OK] norminette`, `[SKIP] cppcheck not installed`, `[OK] compiles clean (-Wall -Wextra -Werror -pthread), no relink`, `[OK] tests passed`.

## make check
Exit 0. Tail: `[OK] sealed acceptance tests intact (73 seal(s))`, `[OK] approved plan intact`, `[OK] Harness invariants OK.` (WARN only: no .harness/templates.json).

## Evidence (bash, directory build/tsan_9.2.1; SRCS=$(find coders/src -name '*.c'))
(a) `cc -Wall -Wextra -Werror -pthread -g -Icoders tests/acceptance/test_9.2.1.c $SRCS -o build/tsan_9.2.1/vg` -> build=0
`timeout 300 valgrind --trace-children=yes --error-exitcode=42 build/tsan_9.2.1/vg` -> valgrind_rc=0
(ERROR SUMMARY: 0 errors in every process; all heap blocks freed.)

(b) `cc -Wall -Wextra -Werror -pthread -g -fsanitize=thread -Icoders tests/acceptance/test_9.2.1.c $SRCS -o build/tsan_9.2.1/tsan` -> build=0
`TSAN_OPTIONS=exitcode=66 timeout 120 setarch -R build/tsan_9.2.1/tsan` -> tsan_rc=0
