# Implementation report - subtask 6.3.4 Take dongles

## Files touched
- `coders/src/dongle/take_dongles.c` (new): `take_dongles` + 4 static helpers
  (`get_dongles`, `release_pair`, `push_requests`, `take_pair`).
- `coders/main.h`: `take_dongles` prototype (Dongle section); 42 `Updated:` refreshed.
- `coders/Makefile`: `src/dongle/take_dongles.c` added to SRC; 42 `Updated:` refreshed.
- `tests/unit/test_take_dongles.c` (new unit test, written first / red).

## Design notes
- Validation (`get_dongles`): NULL coder/taken/sim/dongles, `first < 0`,
  `first >= second` (rejects the one-coder / same-dongle case explicitly, so a
  mutex is never locked or unlocked twice), `second >= number_of_coders`.
  Touches nothing.
- `push_requests`: `build_request` before any lock; lock first then second;
  push into both heaps. Returns 1 (held+queued), -1 (nothing queued, released),
  -2 (fatal: second push failed, or an unlock failed).
- `take_pair` (both held): stopped -> 0 with out=0; clock failure -> -1;
  not ready -> `wait_for_dongles` (1 on success, -2 if it failed: it already
  released everything); ready -> pop both, `available = 0`, out=1.
- Tail: release both (both attempted, results checked); any fatal ->
  `set_stopped` + `wake_dongles`, return -1 holding nothing; `*taken` written
  only on return 0.
- Only `stop_mutex` is taken under a dongle mutex (via `is_stopped`).

## Review round 1 (CHANGES_REQUESTED)
Static helpers renamed to closed-list verbs: `pick_dongles`->`get_dongles`,
`unlock_pair`->`release_pair`, `lock_and_queue`->`push_requests`,
`step`->`take_pair`. Grep found no clash in main.h or tests/ (a test-local
static `push_pair` exists in test_pop_request.c, so `push_pair` was avoided).
42 `Updated:` of take_dongles.c refreshed (13:52:00).

## make check (after rename)
Foreground run, exit 0: norminette OK, build clean, tests passed, harness
invariants OK, src scope OK, seals intact. Acceptance binary (14:01:32) is
newer than the last source edit (take_dongles.c 13:59:39).

## valgrind (--leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42)
Exit 0, `ERROR SUMMARY: 0 errors from 0 contexts`.

## TSan (setarch -R) on the corrected, re-sealed oracle
Built with `cc -g -fsanitize=thread -pthread -Wall -Wextra -Werror -Icoders
tests/acceptance/test_6.3.4.c coders/src/*/*.c`; run under `setarch -R`:
exit 0, empty output, no `WARNING: ThreadSanitizer`. Clean.
