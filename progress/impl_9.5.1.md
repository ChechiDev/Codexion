# Implementation 9.5.1 Print run error

Files touched:
- coders/src/cli/print_run_error.c (new, Stdheader generated with the vim plugin)
- coders/main.h (prototype `void print_run_error(void);` after `print_arg_error`; `Updated:` refreshed via Stdheader)
- coders/Makefile (`src/cli/print_run_error.c` in SRC; `Updated:` refreshed via Stdheader)
- tests/unit/test_print_run_error.c (new, unit test)

Implementation: single function, `fprintf(stderr, "Error: simulation failed\n");`, no args, no state, no allocation, no globals.

## TDD red (before implementing)
Command: `cc -Wall -Wextra -Werror -pthread -Icoders tests/unit/test_print_run_error.c <all coders src except main.c>`
Result: rc=1, `error: implicit declaration of function 'print_run_error'`.

## TDD green / acceptance / valgrind / TSan / norminette (bash, word-splitting on)
- unit test build+run: unit_rc=0
- acceptance test_9.5.1 build+run: acc_rc=0
- `valgrind --trace-children=yes --error-exitcode=42 --leak-check=full` acceptance bin: vg_rc=0 (ERROR SUMMARY: 0 errors); unit bin: vg_unit_rc=0
- TSan (`-fsanitize=thread`, `TSAN_OPTIONS=exitcode=66 setarch -R`): tsan_rc=0
- `norminette coders/`: no non-OK lines, rc=0

## make verify (run alone, `pgrep -a make` empty beforehand)
Tail of output:
    [OK]    norminette
    [SKIP]  cppcheck not installed
    make[1]: Nothing to be done for 'all'.
    [OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
    ...
    test_main_usage: OK
    1000 1 burned out
    [OK]    tests passed
Green (no failure line). Note: the zsh shell did not report the exit status through PIPESTATUS; success is shown by the final `[OK] tests passed`.

Note: at the very start `pgrep -a make` showed a `make check` / `make verify` already running (pids 112954/112964, the pre-existing red run); none was running for the final `make verify`.
