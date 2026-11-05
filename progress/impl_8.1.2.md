# Implementation 8.1.2 get_target_met

Files: coders/src/monitor/get_target_met.c (new, static helper get_count_reached + get_target_met),
coders/main.h (prototype in Monitor section, Updated refreshed), coders/Makefile (SRC + Updated),
tests/unit/test_get_target_met.c (new, written first; red = implicit declaration error, then green).

Validation (NULL sim/met/coders, coders < 1) happens before any lock; required 0 skips the loop (*met = 1, no lock);
each count read under its own coder mutex, one at a time, stops at first below target; any lock/unlock failure -> -1,
*met written only after the last successful unlock.

Results (real runs):
- make check: rc=0 (norminette OK, build OK, unit + acceptance tests OK, harness invariants OK)
- make tsan: "[OK] tsan clean"
- make memcheck: "[OK] valgrind clean"
- valgrind acceptance 8.1.2: 43 allocs, 43 frees, ERROR SUMMARY: 0 errors, exit 0
- TSan build of test_8.1.2.c under setarch -R: rc=0, no ThreadSanitizer warnings
