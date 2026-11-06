# Impl ISS-32.1 sleep_ms stop result

## Files touched
- coders/src/time/sleep_ms.c: `is_stopped` branch now returns 1 (was -1); clock failures still -1; header `Updated:` refreshed.
- tests/unit/test_sleep_ms.c: stopped case renamed `test_stopped_returns_one_fast`; expects 1 for `LONG_MAX` and for 1000.
- coders/main.h, run_coder.c, acceptance tests: untouched.

## Red
Before the change: `tests/unit/test_sleep_ms.c:58: test_stopped_returns_one_fast: Assertion 'sleep_ms(&sim, LONG_MAX) == 1' failed.` (rc 134)

## Green
`make verify`: rc=0
```
[OK]    norminette
[SKIP]  cppcheck not installed
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
test_main_usage: OK
[OK]    tests passed
```
`make check`: rc=0, all sections OK (plan sync, gate consistency, sealed oracles intact, src scope within ISS-32.1 files, "Harness invariants OK"); only a WARN about .harness/templates.json (unrelated).
