# Implementation 10.1.1 Sleep start turn

## Changes
- `coders/src/coder/sleep_start_turn.c` (new): `int sleep_start_turn(t_coder *coder)`. -1 for NULL coder or NULL sim; odd id returns 0 (no clock, no sleep); even id returns `sleep_ms(coder->sim, START_TURN_MS)` unchanged. Ids are seat numbers 1..N. Header generated with Stdheader.
- `coders/main.h`: `# define START_TURN_MS 1` in Macros, prototype in Coder section; `Updated:` refreshed via the plugin date.
- `coders/Makefile`: added `src/coder/sleep_start_turn.c`; `Updated:` refreshed.
- `tests/unit/test_sleep_start_turn.c` (new): invalid pointers, odd seats on a stopped sim (return 0), even seat waits >= START_TURN_MS and returns 1 when stopped.
- Not touched: `run_coder`, `tests/acceptance/`.

Note: the Stdheader dates were generated on 2026/11/09 19:41:30 (header date of the scratch file); `Updated:` of main.h and Makefile was set to that same value by editing only the timestamp portion of the plugin output.

## TDD evidence
Red (before implementing):
```
tests/unit/test_sleep_start_turn.c:21:16: error: implicit declaration of function 'sleep_start_turn' [-Werror=implicit-function-declaration]
```
Green: unit test and acceptance test pass after implementation (see below).

## make verify
`make verify` exit 0 (run with no other make running); output:
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
... (scenario output) ...
test_main_usage: OK
1000 1 burned out
[OK]    tests passed
```
(A first `pgrep -a make` showed `make check` / `make verify` processes owned by the harness hooks, not by me; verify ran to completion and was re-run: exit 0.)

## norminette
`norminette coders/src/coder/sleep_start_turn.c coders/main.h` -> `sleep_start_turn.c: OK!`, `main.h: OK!` (norminette does not accept the Makefile as a target; `make lint` = `norminette coders/` passed within verify).

## Valgrind / TSan (bash, acceptance test_10.1.1, objects from build/obj/coders)
- Build: `cc -Wall -Wextra -Werror -pthread -Icoders tests/acceptance/test_10.1.1.c $OBJS -o acc`
- `valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --trace-children=yes --error-exitcode=42 -q ./acc` -> exit 0
- `cc ... -fsanitize=thread -g tests/acceptance/test_10.1.1.c <all coders/*.c except main.c> -o acc_tsan` then `TSAN_OPTIONS=exitcode=66 setarch $(uname -m) -R ./acc_tsan` -> exit 0
- Unit binary `test_sleep_start_turn` -> exit 0
