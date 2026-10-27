# Implementation 2.2.10 Is digit

## Files touched
- created `coders/src/utils/is_digit.c` (42 header generated with :Stdheader on a scratch copy)
- modified `coders/main.h` (`is_digit` first in Utils section; Updated line refreshed)
- modified `coders/Makefile` (`src/utils/is_digit.c` in SRC; Updated line refreshed)
- created `tests/unit/test_is_digit.c` (digits 0-9 give 1; '/', ':', '\0', ' ', '+', '-', 'a', (char)-1 give 0)

## Verification
`make verify`: rc=0
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
[OK]    tests passed
```
`make check`: all sections OK (plan sync, gate consistency, docs gate, src scope within
declared files, 16 sealed acceptance tests intact, "Harness invariants OK.").

`make verify` only printed test_main_usage, so I also compiled and ran directly with
`cc -Wall -Wextra -Werror -pthread -Icoders ... coders/src/*/*.c`:
- tests/unit/test_is_digit.c: rc=0
- tests/acceptance/test_2.2.10.c: rc=0 (AC1-AC4, including build, no relink, norminette)

I ran `make -C coders fclean` afterwards, so `coders/codexion` and objects are removed.
