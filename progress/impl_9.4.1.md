# Implementation 9.4.1 Run simulation

## Files touched
- coders/src/lifecycle/run_simulation.c (new; Stdheader-generated header; 2 functions: static `join_simulation`, public `run_simulation`)
- coders/main.h (prototype in the Lifecycle section; `Updated:` line refreshed with Stdheader output)
- coders/Makefile (`src/lifecycle/run_simulation.c` added to SRC; `Updated:` line refreshed)
- tests/unit/test_run_simulation.c (new: NULL args, zero compiles leaves 0x5A storage byte-identical, one real 3-coder run returns 0)

## Inner TDD
Unit test first, compiled before the implementation:
`implicit declaration of function 'run_simulation'` (-Werror). Then implemented; green.

## Behaviour (order per the spec)
sim/config NULL -> -1; zero compiles required -> 0 before touching sim; init_simulation != 0 -> -1 without free_sim;
start_threads -1 -> free_sim, -1; start_threads -2 -> -1 without free/read/join; join_threads != 0 -> -1 without free/read;
after a successful join `failed` is read unlocked into a local, `free_sim` is always called and its result kept in its own local;
returns -1 if either is non-zero, else 0. No allocation, no globals, no printing.

## make verify (rc=0)
```
[OK]    norminette
[SKIP]  cppcheck not installed
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
[OK]    tests passed
```
`make check`: exit 0 (tests passed, gates consistent, src changes within declared files, sealed acceptance intact, approved plan intact, "Harness invariants OK").

## Acceptance binary (tests/acceptance/test_9.4.1.c)
Oracle reads env: CODEXION_TIME_SLACK_MS, CODEXION_UNDER_VALGRIND, TSAN_OPTIONS (the last two only widen the alarm watchdog). All builds in bash, scratch outputs outside build/.

- Native (build/tests/acceptance/test_9.4.1.bin from `make test`, slack default 0): `./build/tests/acceptance/test_9.4.1.bin` -> exit 0
- Valgrind: `cc -Wall -Wextra -Werror -pthread -Icoders -g tests/acceptance/test_9.4.1.c build/obj/coders/src/*/*.o -o <scratch>/acc_vg`, then
  `CODEXION_UNDER_VALGRIND=1 CODEXION_TIME_SLACK_MS=400 valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42 --trace-children=yes -q <scratch>/acc_vg` -> exit 0
- TSan: `cc -Wall -Wextra -Werror -pthread -Icoders -fsanitize=thread -g tests/acceptance/test_9.4.1.c $(find coders -name '*.c' ! -path coders/main.c) -o <scratch>/acc_tsan`, then
  `CODEXION_TIME_SLACK_MS=400 TSAN_OPTIONS=exitcode=66 setarch $(uname -m) -R <scratch>/acc_tsan` -> exit 0
- Extra: unit test under TSan (same recipe, no slack) -> exit 0.

(<scratch> = /tmp/claude-1000/-home-sergi-dev-codexion/a80f3096-b976-437e-a9c1-a127b14e8db4/scratchpad.)
No blockers. Not set done, not committed.
