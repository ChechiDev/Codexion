# Implementation 8.1.1 get_burned_out

## Files touched
- coders/src/monitor/get_burned_out.c (new, 42 header)
- coders/main.h (new `/* ---- Monitor ---- */` section after Coder, Updated: refreshed)
- coders/Makefile (SRC entry, Updated: refreshed)
- tests/unit/test_get_burned_out.c (new unit test)

## Inner TDD
Unit test written first; compiling it before the code failed with
`error: implicit declaration of function 'get_burned_out'` (red). Then the code was written.

## Design
NULL coder / coder->sim / burned return -1 before any lock. Lock coder mutex, read
`last_compile_start` and `get_elapsed_ms(sim->start_ms)` both under it; clock -1 unlocks and
returns -1; comparison `last <= now - time_to_burnout` (no overflow); `*burned` written only
after a successful unlock.

## Verification (real output)
- `make check`: `[OK] norminette`, `[OK] tests passed`, `[OK] src/ changes are within the in_progress/closing subtask's declared files`, `[OK] sealed acceptance tests intact (66 seal(s))`, `[OK] Harness invariants OK.`
- `make tsan`: `[OK]    tsan clean`
- `make memcheck`: `[OK]    valgrind clean`
- Acceptance under valgrind (`--leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42`): `All heap blocks were freed -- no leaks are possible`, `ERROR SUMMARY: 0 errors from 0 contexts`.
- Acceptance built with `-fsanitize=thread` under `setarch -R`: no output, rc=0.
- Acceptance binary plain run: rc=0.
