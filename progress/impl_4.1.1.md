# Impl 4.1.1 Is request before

## Files touched
- coders/src/policy/is_request_before.c (new): lexicographic (key, ticket) with `<` / `>`, NULL -> 0, coder never read, 42 header.
- coders/main.h: `t_request` typedef (Types, after t_sim), new `/* ---- Policy ---- */` section (after Log, before CLI), `Updated:` refreshed.
- coders/Makefile: `src/policy/is_request_before.c` added to SRC, `Updated:` refreshed.
- tests/unit/test_is_request_before.c (new): inner unit test (order, tie-break, strict, extremes, NULL).

## Verification
- `norminette coders/`: all OK.
- `make check`: green (`[OK] norminette`, clean build/no relink, tests passed, harness invariants OK; cppcheck not installed -> SKIP).
- `make tsan`: `[OK] tsan clean`.
- `make memcheck`: `[OK] valgrind clean`.
- Acceptance test_4.1.1.c compiled and run standalone: exit 0 (AC1-AC9, including nm and norminette).
- Unit test compiled and run standalone: exit 0.

Note: the strict red step of the unit test was not observed (a first attempt was blocked by a hook before running); the test was written before the implementation but only run after.
