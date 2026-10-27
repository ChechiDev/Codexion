# Implementation 2.2.9 (BLOCKED on acceptance oracle)

## Files touched
- coders/src/utils/trim_slice.c: both loops now use `is_space(...)`; inline whitespace test removed; `Updated:` header line refreshed via :Stdheader on a scratch copy (18:31:45). Includes unchanged (<stddef.h> for NULL, <string.h> for strlen).

## Blocker
`tests/acceptance/test_2.2.9.c:23` asserts `slice.start == input + 6`, but the input is
`" \t\n\v\f\r a ..."`, which has SEVEN leading whitespace chars (' ', \t, \n, \v, \f, \r, ' '), so the correct offset is `input + 7`.
I compiled the ORIGINAL (HEAD) trim_slice.c with a probe: it also yields offset 7, len 8. So the test fails with the old code too; it is not a regression from my change, the oracle is wrong (should be `input + 7`). I cannot edit the sealed test; Codex must fix it (the `len == 8` assertion is correct).

## make verify (tail)
```
[OK]    norminette
[SKIP]  cppcheck not installed
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
test_2.2.9.bin: tests/acceptance/test_2.2.9.c:23: test_ac1_trim_slice_regression: Assertion `slice.start == input + 6' failed.
[FAIL]  tests/acceptance/test_2.2.9.c
make: *** [Makefile:69: test] Error 1
```
## make check
Harness sections 1-8 OK (plan sync, seals, src scope); exits 1 only because of the test above.

## Resolution (leader)
Codex re-ran the 2.2.9 plan gate (PLAN_APPROVED), fixed the oracle expectation
to `slice.start == input + 7` (`tests/acceptance/test_2.2.9.c:23`) and re-sealed
it. With the unchanged refactor of `coders/src/utils/trim_slice.c`, `make check`
exits 0: norminette, clean build without relink, all unit and acceptance tests green.
