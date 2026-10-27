# impl 2.1.1 Trim span

Files:
- coders/src/parsing/trim_span.c (new: `trim_span` + static `is_space`, six C-locale whitespace bytes, no copy, NULL gives {NULL, 0})
- coders/main.h (`t_span`, `trim_span` prototype)
- coders/Makefile (`src/parsing/trim_span.c` in SRC)
- tests/unit/test_trim_span.c (new unit tests; red first: `t_span` undefined, then green)

`make verify` output:

    [OK]    norminette
    [SKIP]  cppcheck not installed
    make[1]: Nothing to be done for 'all'.
    [OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
    test_main_usage: OK
    [OK]    tests passed

`norminette coders/`: no errors. The acceptance test tests/acceptance/test_2.1.1.c ran in the same loop and passed (the loop would have stopped on failure). Not run: `make check` (harness gate).
