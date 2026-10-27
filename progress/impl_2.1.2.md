# impl 2.1.2 parse_number

Files: coders/src/parsing/parse_number.c (new), coders/main.h (prototype),
coders/Makefile (SRC), tests/unit/test_parse_number.c (new unit tests).

Approach: trim_span, then digits-only loop with `value > (INT_MAX - digit) / 10`
check before each multiply/add; `*out` written only on success.

`make verify` output (tail):
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
[OK]    tests passed
```
`norminette coders/`: no errors. Acceptance test_2.1.2 and unit tests ran in the test target.
Note: the red unit-test run was skipped due to a hook blocking my combined command; the unit test was written before the implementation.
