# Implementation ISS-46.1 Keep expired deadline

## Files touched
- coders/src/coder/set_compile_start.c: under the coder mutex, reads `now`, and writes the field only when `last_compile_start > now - time_to_burnout` (returns 0); otherwise leaves it and returns 1; NULL/lock/clock/unlock failures stay -1 (unlock failure wins). Contract comment 0/1/-1 added. Updated header line refreshed (2026/11/11 12:00:00).
- tests/unit/test_set_compile_start.c: updated to the new contract first (prefill 100 in the store test, new `check_keeps_expired_deadline` covering 1 on reached/inclusive boundary with field kept, 0 just after, mutex unlocked).
- run_coder.c, get_burned_out.c, run_monitor.c: not modified. Acceptance tests: not edited.

## make verify (exit 0)
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
... (simulation logs, test_main_usage: OK, "1000 1 burned out")
[OK]    tests passed
```
Includes tests/acceptance/test_ISS-46.1.c and the migrated test_7.1.1.c (green).

## ISS-46.1 acceptance binary (all coders/src/*.c, run from repo root)
- plain: rc=0
- valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42: rc=0, 40 allocs / 40 frees, ERROR SUMMARY: 0 errors
- -fsanitize=thread under `setarch -R`: rc=0, no ThreadSanitizer warning output

## Code gate r1 fix
tests/unit/test_set_compile_start.c: live-deadline prefill changed 201 -> 400 (margin like the acceptance test); the exact inclusive boundary assertion at 200 is kept. Re-run `make verify` (exit 0):
```
[OK]    norminette
[SKIP]  cppcheck not installed
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
1000 1 burned out
[OK]    tests passed
```

Pending Codex review; not closed, not committed.
