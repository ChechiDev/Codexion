# Implementation 7.1.1 Set compile start

Files touched:
- coders/src/coder/set_compile_start.c (new, 42 header)
- coders/main.h (new `/* ---- Coder ---- */` section after Dongle, Updated: refreshed)
- coders/Makefile (SRC entry, Updated: refreshed)
- tests/unit/test_set_compile_start.c (new unit test: NULL inputs, store + unlock)

Design: NULL coder/sim -> -1 before any lock; lock coder mutex; read
get_elapsed_ms(sim->start_ms) under the lock; -1 clock -> unlock, return -1,
field unchanged; assign field; unlock (failure -> -1, no rollback). Only the
coder's own mutex is taken.

Verification (all run in foreground, sequentially):
- `make check` -> exit 0 (harness invariants OK, acceptance + unit + lint green).
- `norminette coders/` -> no non-OK lines.
- build/tests/acceptance/test_7.1.1.bin (timestamp 18:31:30, newer than source 18:24:08) -> exit 0.
- valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all
  --error-exitcode=42 on the acceptance binary -> exit 0, "All heap blocks were
  freed", 0 errors.
- Acceptance test built with -fsanitize=thread, run under `setarch -R` -> exit 0,
  no ThreadSanitizer warnings.
- Unit test under valgrind -> exit 0.

Skipped: nothing. Clock/lock/unlock failure paths have no AC (code review only).
