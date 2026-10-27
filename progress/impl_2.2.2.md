# impl 2.2.2 - move trim_span to utils

Files: coders/src/utils/trim_span.c (new, verbatim body, fresh :Stdheader),
coders/Makefile (SRC swap + Updated), coders/src/parsing/trim_span.c (deleted by user).
Diff vs HEAD parsing/trim_span.c: only Created/Updated header lines differ.

## make verify (real output, tail)
norminette OK; build OK (-Wall -Wextra -Werror -pthread), no relink OK;
test_main_usage: OK
test_1.1.1.bin: test_1.1.1.c:185: test_ac4_clean_preserves_binary: Assertion `object_count() == 0' failed.
[FAIL] tests/acceptance/test_1.1.1.c

## Cause (environment, not code)
A stale untracked build artifact `coders/src/parsing/trim_span.o` (left from the old
layout) remains. `make fclean` only removes the objects listed in the new SRC, so it
survives and 1.1.1's object_count() counts it. My shell cannot delete files under
coders/src (guard hook). Fix: `rm coders/src/parsing/trim_span.o`, then rerun
`make verify` / `make check`. The test run aborts at 1.1.1, so 2.1.1 and 2.2.2
acceptance were not reached yet.

## make check
Harness invariants OK (plan sync, seals, src scope) but exits Error 1 because of the failing tests above.

## Final verification (leader, after the user removed the stale object)
The user ran `rm coders/src/parsing/trim_span.o` (2026-10-27). Re-run of
`make check`: `[OK] norminette`, `[OK] compiles clean (-Wall -Wextra -Werror -pthread), no relink`,
`[OK] tests passed` (all unit and acceptance tests, including 1.1.1, 2.1.1 and 2.2.2),
`[OK] Harness invariants OK`; exit code 0.
