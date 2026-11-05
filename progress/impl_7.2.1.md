# Implementation 7.2.1 Take coder dongles

## Files touched
- coders/src/coder/take_coder_dongles.c (new, 4 functions: print_taken, release_taken, take_pair, take_coder_dongles)
- coders/main.h (prototype in Coder section, Updated line refreshed)
- coders/Makefile (SRC entry, Updated line refreshed)
- tests/unit/test_take_coder_dongles.c (new unit test: invalid input, pair and single take, stop, ticket exhaustion)

## Design
Invalid input -> -1 first. take_single_dongle (same pointer) or take_dongles once, with a local `got`
(caller's `*taken` untouched on failure). -1 from them -> -1 with no cleanup. got == 0 -> `*taken = 0`, 0.
Then print_state once (single) or twice (pair), stopping at the first -1; on -1 release_dongles exactly once:
release -1 -> -1; else is_stopped true -> `*taken = 0`, 0; else -1. `*taken = 1` only after all logs.
No mutex locked here, no set_stopped/wake_dongles, no clock, no allocation, no global.

## Verification (real results)
- `make check` (foreground): exit 0, norminette OK, build clean, tests passed (acceptance bin
  build/tests/acceptance/test_7.2.1.bin built 11:22:58, newer than source 11:15:34), integrity OK.
  cppcheck is not installed (skipped by make check).
- Acceptance under valgrind (`--leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all
  --error-exitcode=42 --trace-children=yes`): rc=0, 0 errors, no leaks in parent and both forked children.
- Acceptance and unit built with `-fsanitize=thread`, run under `setarch -R`: rc=0, no ThreadSanitizer warnings.
- Unit test under valgrind: rc=0.
- Skipped: `make tsan` / `make memcheck` targets not run separately (equivalent manual runs above).
