# ISS-14.1 Report invalid arguments as flags
- status: done
- layer: core
- depends: 2.3.3, 2.4.2
- files:
  - coders/src/parsing/parse_config.c  # modifica: parse_config fills int *invalid and returns the invalid count
  - coders/main.h                      # modifica: parse_config prototype (Parsing section)
  - coders/main.c                      # modifica: int invalid[9] on the stack, print_arg_errors tests invalid[position]
- migrates:
  - tests/acceptance/test_2.3.3.c
- acceptance:
  - AC1 | `parse_config(argv, &config, invalid)` on `{"codexion","4","800","200","200","200","3","50","edf",NULL}` returns 0, sets `invalid[1]` to `invalid[8]` to 0 and fills the eight `t_config` fields with 4, 800, 200, 200, 200, 3, 50 and `MODE_EDF` | tests/acceptance/test_ISS-14.1.c
  - AC2 | with `invalid[1]` to `invalid[8]` pre-filled with a garbage value (42), a call with only positions 2, 3 and 8 invalid returns 3 and leaves exactly `invalid[2]`, `invalid[3]` and `invalid[8]` at 1 and the other five positions at 0, so the caller never has to zero the array | tests/acceptance/test_ISS-14.1.c
  - AC3 | all eight arguments invalid returns 8 with `invalid[1]` to `invalid[8]` all at 1; a sentinel stored in `invalid[0]` before the call is unchanged after it, in this and every other case | tests/acceptance/test_ISS-14.1.c
  - AC4 | a single invalid argument at each position `pos` in 1..8 (numeric values `"-1"`, `"abc"`, `""`, `"2147483648"`; scheduler `"FIFO"`, `" edf"`, `"rr"`), all others valid, returns 1 with only `invalid[pos]` set to 1; every `argv` string is byte-for-byte unchanged after the call | tests/acceptance/test_ISS-14.1.c
  - AC5 | the binary is unchanged for the user: `./codexion 4 -5 abc 200 200 3 50 rr` writes the error lines for positions 2, 3 and 8 in that order, then the usage exactly once, nothing to stdout, exit status 1; a single invalid argument writes its own error line then the usage, status 1; `./codexion 4 800 200 200 200 3 50 edf` exits 0 with empty stderr | tests/acceptance/test_ISS-14.1.c
  - AC6 | `make -C coders` builds `codexion`, `norminette coders/` reports no error, and an invalid run under valgrind reports no leak and no error | tests/acceptance/test_ISS-14.1.c

Code review check: no bitwise operator (shift, bitwise or, bitwise and) on argument positions remains in `coders/main.c` or `coders/src/parsing/parse_config.c`; `coders/main.h` declares exactly `int parse_config(char **argv, t_config *config, int *invalid);` in the Parsing section; `invalid[0]` is never written; positions 1..7 still go through `static parse_field` (`parse_number` + `get_arg_minimum`) and position 8 through `parse_mode`, with no validation rule duplicated or changed; no early stop; `parse_config.c` prints nothing; `main.c` uses a stack `int invalid[9]` (no allocation) and `print_arg_errors(const int *invalid)`; the sealed `tests/acceptance/test_2.4.2.c` is not modified and still passes; the 42 header `Updated:` lines of the three files are refreshed with Stdheader; Norm respected.

## Description

GitHub issue #14. The bit mask that `parse_config` returns asks the reader to
know bitwise operators to follow plain argument validation; it is replaced by
an explicit array of flags.

**Not a structural refactor.** The parser's public C API changes (three
parameters instead of two, a count instead of a mask), so the new API tests
(AC1-AC4) and the migrated `test_2.3.3.c` are red at the plan gate, as for any
new behaviour. Only the binary's user-visible behaviour is unchanged: AC5 checks
it in this subtask's oracle, and the sealed `test_2.4.2.c` stays untouched as
extra regression evidence.

- **New signature:** `int parse_config(char **argv, t_config *config, int *invalid);`
  `invalid` points to 9 ints owned by the caller; index 0 is unused and never
  written. The loop runs positions 1..8 and writes `invalid[position]` as 0 or
  1 for **every** position, so the caller does not zero the array first.
- **Return value:** the number of invalid arguments (0..8); 0 means the config
  is valid. When it is non-zero, `*config` contents are unspecified and must not
  be used (same contract as before).
- **Unchanged internals:** positions 1..7 still go through `static parse_field`,
  position 8 through `parse_mode`. Every argument is still checked (no early
  stop), so the user sees all mistakes at once.
- **`main.c`:** declares `int invalid[9]` on the stack, `print_arg_errors(const
  int *invalid)` walks positions 1..8 and calls `print_arg_error(position)` when
  `invalid[position]` is set; then `print_usage()` once and `EXIT_USAGE`, as in 2.4.2.
- **Layering:** `parsing/` stays pure (prints nothing); `cli/` owns the messages.

## Why one subtask (user decision, 2026-11-02)

The atomic unit here is one function contract: the `parse_config` prototype
and its **only** caller. `main.c`'s change is nothing more than adapting that
call (pass the array, test `invalid[position]`). Changing the prototype in
`main.h` without `main.c` does not compile, so a split could only build through
a temporary bridge in `main.c` that converts `invalid[]` back into a bit mask,
written in one subtask and deleted in the next: exactly the bitwise code this
issue removes. One subtask keeps every gate green, stays within 3 declared
files, and leaves no throwaway code.

## Migration

`tests/acceptance/test_2.3.3.c` (sealed) asserts mask values (`1 << pos`,
`0x10c`, `0x1fe`, `(1 << 2) | (1 << 8)`). At the plan gate Codex migrates each
assertion one-to-one to the equivalent `invalid[]` check plus return count, and
re-seals it. The unit test `tests/unit/test_parse_config.c` is rewritten by the
implementer for the new signature (inner TDD).

## Documentation (Codex, at close)

`docs/reference/2.3.3-parse-config.md`, `docs/reference/2.4.2-validate-arguments.md`
and the mask mention in `docs/reference/2.4.1-print-arg-error.md` describe the
mask; they are updated to the flag array. The original specs in
`docs/plan/phase-02-*` stay as the historical record.
