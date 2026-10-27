# Implementation 2.2.8 Is space

Files touched:
- coders/src/utils/is_space.c (new, 42 header from :Stdheader)
- coders/main.h (is_space prototype before trim_slice; Updated refreshed)
- coders/Makefile (src/utils/is_space.c in SRC; Updated refreshed)
- tests/unit/test_is_space.c (new, unit test)

Output of `make verify` (after `make -C coders fclean`):
[OK]    norminette
[SKIP]  cppcheck not installed
cc ... -c src/utils/is_space.c -o src/utils/is_space.o (and the rest)
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
[OK]    tests passed
(unit and acceptance test binaries, incl. test_2.2.8.c, are silent on success)

`make check`: all [OK], harness invariants OK.
