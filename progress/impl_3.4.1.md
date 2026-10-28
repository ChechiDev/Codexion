# Impl 3.4.1 Build number text

Files touched:
- coders/src/utils/build_number_text.c (new, 42 header via :Stdheader; static helper `get_digit_count`)
- coders/main.h (NUMBER_TEXT_MAX 20 in Macros; prototype in Utils)
- coders/Makefile (SRC)
- tests/unit/test_build_number_text.c (new unit test)

make verify (tail):
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
[OK]    tests passed
```
make check: all sections OK ("Harness invariants OK."). norminette coders/: no errors.
build/tests/acceptance/test_3.4.1.bin run directly: rc=0.

Round 2: refreshed the `Updated:` header line in coders/main.h and coders/Makefile (generated with Stdheader on a scratch copy). make check green; norminette coders/ clean.
