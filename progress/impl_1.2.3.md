# impl 1.2.3 Argument count and usage

Files touched: `coders/main.c`, `coders/main.h`, `README.md` (42 header
`Updated:` refreshed with the plugin in main.c and main.h, `Created:` kept).

- `main.c`: `#include <stdio.h>`, `print_usage()` (fprintf to stderr), `main(argc, argv)`
  returns `EXIT_USAGE` when `argc != 9`; `(void)argv;`.
- `main.h`: `# define EXIT_USAGE 1`, `void print_usage(void);`, no system include.
- `README.md`: "Running it" under Instructions (command, eight arguments explained,
  zero-argument run with the exact usage output).

## Real output

norminette: `main.c: OK!` / `main.h: OK!`

Usage output (`./coders/codexion`, rc=1):

    Usage: ./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
      scheduler is either fifo or edf

`make verify`: `[OK] norminette`, `[SKIP] cppcheck not installed`,
`[OK] compiles clean ... no relink`, `[OK] tests passed`.

`make check`: all `[OK]`, `Harness invariants OK.` (sealed tests intact).

CI simulation (clean copy without `harness/` and `.claude/`, `CODEXION_TIME_SLACK_MS=40`):
- `make verify`: norminette ok, compiles clean, tests passed
- `make memcheck`: `[ok] valgrind clean`
- `make tsan`: `[ok] tsan clean`

## Review fixes

Finding 1: added `tests/unit/test_main_usage.c`. It renames the entry point
(`#define main codexion_main`), includes `main.c`, captures stdout/stderr via
`dup2` onto `tmpfile()`s, and asserts: argc 1, 8, 10 return `EXIT_USAGE` (1) with
usage on stderr and empty stdout; argc 9 returns 0 with no output; `print_usage()`
starts with `Usage:`, lists the eight arguments in order, mentions `fifo` and `edf`.
No `coders/` or acceptance file touched.

Outputs:
- `make verify`: norminette OK, compiles clean, `test_main_usage: OK`, tests passed.
- `make check`: Harness invariants OK (4 acceptance seals intact).
- CI simulation (clean copy without harness/ and .claude/, CODEXION_TIME_SLACK_MS=40):
  `make verify` OK (tests passed), `make memcheck` valgrind clean, `make tsan` tsan clean.
