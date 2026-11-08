# Implementation 9.2.2 - Coder reports failure

## Files touched
- coders/src/coder/run_coder.c (5 static/public functions: end_run, sleep_compile, sleep_rest, wait_single, run_coder; `Updated:` header edited by hand because Stdheader is not installed here)
- tests/unit/test_run_coder.c (asserts `failed == 0` on the already-stopped path and `failed == 1` on the count failure)

## Code gate rename (second pass)
The static `end_run` was renamed `release_run` (verb `release_` from docs/conventions.md); definition and 3 call sites; `Updated:` header refreshed to 2026/11/08 14:03:53. No behaviour change. Re-run results: `make verify` rc=0 (norminette OK, tests passed); `make check` rc=0 (an earlier attempt failed in test_1.1.1 and test_4.3.1 only because another `make check` was running concurrently in this repo and sharing build/; re-run alone after it ended: green); valgrind run: 0 errors, rc=0; TSan run (`TSAN_OPTIONS=exitcode=66 setarch -R`): rc=0. Results below describe the same commands as in the first pass.

## Design
- Helper results classified: 1 = normal end, -1 = failure. `sleep_compile` / `sleep_rest` return the first non-zero result unchanged; `sleep_rest` ends with `is_stopped` (1 = normal end of cycle).
- `end_run(coder, held, res)`: normal end releases once (held cleared first), a -1 from that release turns it into a failure; failure order: `set_failed`, release (if still held), `set_stopped`, `wake_dongles`. Always returns NULL.
- `wait_single` keeps the `sleep_ms` result (loop while 0).

## make verify
norminette OK, cppcheck skipped (not installed), compiles clean, no relink, tests passed. Exit status 0.

## make check
Exit status 0 (all gates OK; only warn: no .harness/templates.json migration level, pre-existing).

## Evidence (bash, build/tsan_9.2.2, sources = test + all coders/src/**/*.c via find)
(a) `cc -Wall -Wextra -Werror -pthread -g -Icoders tests/acceptance/test_9.2.2.c <all coders/src .c> -o build/tsan_9.2.2/vg` -> build rc=0
`env CODEXION_UNDER_VALGRIND=1 timeout 400 valgrind --trace-children=yes --error-exitcode=42 build/tsan_9.2.2/vg` -> ERROR SUMMARY: 0 errors, all heap blocks freed, rc=0
(b) same plus `-fsanitize=thread` -> `.../tsan`, build rc=0
`TSAN_OPTIONS=exitcode=66 timeout 200 setarch -R build/tsan_9.2.2/tsan` -> no output, rc=0
