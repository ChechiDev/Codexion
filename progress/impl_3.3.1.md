# Implementation 3.3.1 Sleep ms

Files touched:
- coders/src/time/sleep_ms.c (new, 42 header via Stdheader; `sleep_ms` + static `get_slice_us`)
- coders/main.h (`SLEEP_SLICE_US 500` in Macros, `sleep_ms` prototype in Time)
- coders/Makefile (src/time/sleep_ms.c in SRC)
- tests/unit/test_sleep_ms.c (new unit tests: ms<=0, duration, stopped)

Design: ms<=0 -> 0 with no calls; every get_time_ms checked for -1; is_stopped
first in each iteration; remaining ms capped (> SLEEP_SLICE_US) before *1000;
clock re-read after each slice; usleep result ignored; no mutex held across usleep.

Results:
- make verify: norminette OK, build clean, tests passed (output tail above in run: [OK] norminette, [OK] compiles clean, [OK] tests passed)
- make check: Harness invariants OK
- norminette coders/: no errors
- build/tests/acceptance/test_3.3.1.bin: rc=0, also rc=0 with CODEXION_TIME_SLACK_MS=40

## Fix after Codex code gate (CHANGES_REQUESTED)
sleep_ms.c loop now uses `get_elapsed_ms(start)` (-1 on clock failure, never
negative otherwise); `elapsed >= ms` returns 0, else slice from `ms - elapsed`
(0 <= elapsed < ms, no overflow even for ms == LONG_MAX after a backward jump).
Stop check still first. Re-run: make verify OK, make check OK, norminette clean,
acceptance bin rc=0 with and without CODEXION_TIME_SLACK_MS=40.
