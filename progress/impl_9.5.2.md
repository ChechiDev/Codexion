# Implementation 9.5.2 Main runs simulation

## Files touched
- `coders/main.c`: `t_sim sim` on main's stack; after `parse_config` succeeds, `run_simulation(&sim, &config)`; on -1 `print_run_error()` and `return (EXIT_RUNTIME)`; else `return (0)`. Header `Updated:` refreshed with Stdheader (scratch file, 2026/11/09 13:53:36). `main` is 25 lines or fewer, 2 functions in the file, no `exit()`.
- `coders/main.h`: `# define EXIT_RUNTIME 2` after `EXIT_USAGE`; nothing else; `Updated:` refreshed with Stdheader.
- `tests/unit/test_main_usage.c`: `test_exact_argument_count` asserted empty stdout for a valid run (true before wiring). It now asserts non-empty stdout, since main really runs the simulation. No new unit test: the sealed acceptance test is the red/green driver for this wiring.
- README.md not touched (Codex at close).

## Starting red
`make check` (bash, rc=2): `test_9.5.2.bin: tests/acceptance/test_9.5.2.c:215: test_ac1_target_met: Assertion 'run.status == 0 && run.out_len > 0 && run.err[0] == '\0'' failed.` then `[FAIL]  tests/acceptance/test_9.5.2.c`.

## Green
After the edits, the first `make verify` was red on `tests/unit/test_main_usage.c:117 test_exact_argument_count: Assertion 'cap.out[0] == '\0'' failed` (old expectation, see above). After the fix `make verify` (bash): `verify_rc=0`, ending with
```
test_main_usage: OK
1000 1 burned out
[OK]    tests passed
verify_rc=0
```
(includes sealed test_9.5.2, test_2.4.2, test_ISS-14.1). No parallel make was running.

## norminette
`norminette coders/` -> `norm_rc=0`, every file `OK!`.

## valgrind
Literally running the acceptance binary under `valgrind --trace-children=yes --error-exitcode=42 build/tests/acceptance/test_9.5.2.bin` is not meaningful: the oracle itself spawns `coders/codexion` and runs its own valgrind in AC7, so the nested run makes AC1 miss its watchdog (rc=134, assertion at test_9.5.2.c:215; valgrind reported 0 errors, no leaks for the test process). The acceptance binary already passes in `make verify`, including AC7 (valgrind leak check on AC1, AC2, AC4). Equivalent direct runs on the full binary with `--leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42`:
```
[4 5000 200 200 200 3 50 fifo] rc=0 ERROR SUMMARY: 0 errors from 0 contexts
[2 300 400 100 100 3 0 fifo] rc=0 ERROR SUMMARY: 0 errors from 0 contexts
[4 800 200 200 200 0 50 fifo] rc=0 ERROR SUMMARY: 0 errors from 0 contexts
```

## TSan (full binary, `-fsanitize=thread`, built in a mktemp dir, bash, `TSAN_OPTIONS=exitcode=66 setarch -R`)
Build rc=0.
```
[4 5000 200 200 200 3 50 fifo] rc=0 lines=59 tsan_warn=0 stderr_bytes=0   (last: 2953 4 is debugging)
[2 300 400 100 100 3 0 fifo]   rc=0 lines=4  tsan_warn=0 stderr_bytes=0   (last: 301 1 burned out)
[1 300 200 200 200 3 50 edf]   rc=0 lines=2  tsan_warn=0 stderr_bytes=0   (last: 300 1 burned out)
```
