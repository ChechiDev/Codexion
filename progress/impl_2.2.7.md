# Implementation 2.2.7 Remove trim span

Files touched:
- coders/main.h: removed s_span/t_span and trim_span prototype; Updated line from :Stdheader (2026/10/27 18:18:29).
- coders/Makefile: removed src/utils/trim_span.c from SRC; Updated line from :Stdheader.
- tests/unit/test_trim_slice.c: absorbed the 4 cases of the old trim_span unit test (single char, only leading/trailing, blank non-null, non-space controls kept), now on trim_slice.
- tests/unit/test_trim_span.c: deleted (rm succeeded). coders/src/utils/trim_span.c was deleted by the user beforehand.

No reference to span/t_span/trim_span remains in coders/, tests/unit/ or the root Makefile.

make verify (real output):
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
[OK]    tests passed
```
make check: all sections OK (feature_list valid, gates consistent, docs complete, src scope within declared files, 13 seals intact, "Harness invariants OK").
