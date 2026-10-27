# Implementation 2.2.3 Header sections

Files touched: `coders/main.h` only (reordered into Macros, Types, Utils,
Parsing, CLI sections; `Updated:` header line set to 2026/10/27 17:31:16).
Nothing renamed, added or removed; no system includes.

## Output
`make verify` (rc=0):
```
[OK]    norminette
[SKIP]  cppcheck not installed
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
[OK]    tests passed
```
`make check` (rc=0): all sections OK, "Harness invariants OK."

Acceptance `tests/acceptance/test_2.2.3.c` compiled by hand
(`cc -Icoders ... coders/src/*/*.c`) and run from the repo root: rc=0
(make verify does not print this test by name).
