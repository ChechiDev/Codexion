# Implementation 10.1.2 Run coder start turn

## Changes
- `coders/src/coder/run_coder.c`: `res = sleep_start_turn(coder);` once, after `held = 0` and before the loop; loop is `while (res == 0)`; the former trailing `if (res != 0) return (...)` is replaced by a single `return (release_run(coder, &held, res));` after the loop. Still 5 functions; `run_coder` within 25 lines (norminette OK). Cycle, logs and arbitration untouched.
- `tests/unit/test_run_coder.c`: new `check_start_turn_order` (stopped sim: even coder 1 returns NULL, `failed == 0`, `next_ticket == 0`; odd coder 0 then advances `next_ticket` to 1).
- 42 header `Updated:` refreshed by the Stdheader plugin itself, run directly on the real file (not hand-edited, no pasted timestamp): `vim -u NONE -N -es --cmd 'set rtp+=~/.vim/pack/42/start/42header' -c "let g:user42='sperez-l'" -c "let g:mail42='sperez-l@student.42barcelona.com'" -c 'runtime! plugin/stdheader.vim' -c 'normal! Go' -c 'normal! dd' -c 'wq' coders/src/coder/run_coder.c`. The plugin only refreshes line 9 when the buffer is modified, hence the no-op `Go`/`dd`. Result: `Updated: 2026/11/09 20:36:08` (`date` at that moment: 20:36:08). `Created` unchanged.
- README.md, tests/acceptance/ and the rest of coders/ untouched.

## TDD evidence
Red (before the change), `make verify`:
```
test_run_coder.bin: tests/unit/test_run_coder.c:71: check_start_turn_order: Assertion `sim.next_ticket == 0' failed.
Aborted (core dumped)
[FAIL]  tests/unit/test_run_coder.c
make: *** [Makefile:69: test] Error 1
```
Green after the change: see below.

## make verify (after change), exit status 0
```
[OK]    norminette
[SKIP]  cppcheck not installed
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
...
test_main_usage: OK
[OK]    tests passed
```
(`pgrep -a make` checked empty before each run.) Includes acceptance test_10.1.2.c.

## Norminette
`norminette coders/src/coder/run_coder.c` -> `run_coder.c: OK!`, rc 0.

## Valgrind and TSan on the acceptance binary (bash, word-split OBJ)
Build: `OBJ=$(ls build/obj/coders/src/*/*.o)`; `cc -Wall -Wextra -Werror -pthread -Icoders -g tests/acceptance/test_10.1.2.c $OBJ -o acc.bin`.
- Valgrind: `CODEXION_UNDER_VALGRIND=1 CODEXION_BIN=coders/codexion valgrind --trace-children=yes --error-exitcode=42 --leak-check=full acc.bin` -> rc 0, "All heap blocks were freed", ERROR SUMMARY 0 errors.
- TSan: `cc -Wall -Wextra -Werror -pthread -Icoders -fsanitize=thread -g tests/acceptance/test_10.1.2.c coders/src/*/*.c -o acc_tsan.bin`; `TSAN_OPTIONS=exitcode=66 CODEXION_BIN=coders/codexion setarch -R acc_tsan.bin` -> rc 0, no reports.
(AC4's child `coders/codexion` is the normal build, not TSan-instrumented.)

## Not done (leader's)
The ten empirical burnout runs of `5 800` and `5 1000`.

## Empirical runs (leader, code gate duty; evidence, not an acceptance criterion)
Ten runs each, on the local WSL2 machine, normal build, after `make check` exit 0. Count = runs with at least one `burned out` line.
- `./coders/codexion 5 800 200 200 200 3 0 edf` -> burnouts in 1/10 runs (before this change: every run). Exit status 0 in all ten.
- `./coders/codexion 5 1000 200 200 200 3 0 edf` -> burnouts in 0/10 runs. Exit status 0 in all ten.
`5 800` is below the PLAN.md sufficient bound (B >= 600 + 5 * 200 + 10 = 1610), so survival is not guaranteed there; the 1/10 burnout is reported as observed and was not investigated further.
