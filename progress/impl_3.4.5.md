# Implementation 3.4.5 - Declare print log line

Files touched: `coders/main.h` only.
- Added `int print_log_line(t_sim *sim, int coder, const char *text);` in the Log section with a Norm-style contract comment.
- 42 header `Updated:` refreshed via Stdheader (2026/10/29 12:07:43).

Verification:
- `norminette coders/`: all OK (no errors).
- `build/tests/acceptance/test_3.4.5.bin` and `test_3.4.3.bin`: exit 0.
- `make check`: rc=0, no FAIL, harness invariants OK.
- Note: the first `make check` after the header edit failed once in test_1.1.1 (`coders/codexion` missing right after the header-triggered rebuild/clean in a previous test run); a rerun was fully green, twice.
