# Impl 2.2.6 — Move trim logic

Files touched:
- coders/src/utils/trim_slice.c: now owns the algorithm (NULL -> {NULL,0}, inline whitespace test, (int)strlen, trailing step-back). Header Updated refreshed with :Stdheader.
- coders/src/utils/trim_span.c: thin delegate to trim_slice; static is_space removed. Header refreshed.
- tests/unit/test_trim_slice.c: added test_control_chars_kept ('\x01'/'\x7f' not trimmed).

Result: `make verify` rc=0 (norminette OK, compiles clean -Wall -Wextra -Werror -pthread, no relink, tests passed);
`make check` rc=0 (harness invariants OK, sealed acceptance intact). cppcheck skipped (not installed).
