# Implementation 9.1.2 Set failed

Files touched:
- coders/src/state/set_failed.c (new): locks stop_mutex only, sets failed = 1, unlocks.
- coders/main.h: prototype `int set_failed(t_sim *sim);` in State section; Updated line refreshed.
- coders/Makefile: src/state/set_failed.c added to SRC; Updated line refreshed.
- tests/unit/test_set_failed.c (new): success/idempotent/no-stop, lock failure.

Results:
- `make verify`: `[OK] norminette`, `[OK] compiles clean (-Wall -Wextra -Werror -pthread), no relink`, `[OK] tests passed` (cppcheck not installed, skipped).
- `make check`: `[OK] Harness invariants OK.` (only WARN: missing .harness/templates.json, pre-existing).
- Acceptance tests/acceptance/test_9.1.2.c and unit test built and run manually: PASS both.

Sanitizer evidence for the acceptance binary (leader, run outside the Codex sandbox, 2026-11-08; requested by the first code gate):
- Sources: `tests/acceptance/test_9.1.2.c` plus every `coders/src/**/*.c`, flags `-Wall -Wextra -Werror -pthread -g -Icoders`.
- `valgrind --leak-check=full --error-exitcode=42 build/tsan_9.1.2/acc_plain` -> exit status 0. Summary: ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0). Log: `build/tsan_9.1.2/valgrind.log`.
- Full rebuild with `-fsanitize=thread`; `TSAN_OPTIONS=exitcode=66 setarch -R build/tsan_9.1.2/acc_tsan` -> exit status 0; `WARNING: ThreadSanitizer` count: 0. Log: `build/tsan_9.1.2/tsan.log`.
