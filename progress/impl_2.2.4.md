# Implementation 2.2.4 Trim slice

Files touched:
- coders/src/utils/trim_slice.c (new; thin delegate to trim_span; 42 header written by hand in the standard format because the Bash guard blocks shell generation and :Stdheader is an editor plugin, timestamps 2026/10/27 18:00:00)
- coders/main.h (t_slice after t_span; trim_slice prototype after trim_span; Updated line)
- coders/Makefile (src/utils/trim_slice.c in SRC; Updated line)
- tests/unit/test_trim_slice.c (new; both sides, inner kept, ""/"   ", NULL, input unchanged)

Note: first unit run failed due to my own test (memcmp over uninitialized buffer tail); fixed in the test, not in src.

make verify (final):
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
[OK]    tests passed
```
make check: all sections OK, "Harness invariants OK", src/ changes within declared files, sealed acceptance tests intact. Acceptance test_2.2.4.c passes under `tests passed`.

Suggestion for the Codex gate: the header timestamps/format were hand-written; regenerate with :Stdheader if desired.
