# Implementation 2.4.1 Print argument error

## Files touched
- coders/src/cli/print_arg_error.c (new): `print_arg_error(int position)`; static const name table, `get_arg_minimum` for min, `INT_MAX` for max, fprintf to stderr; position 8 scheduler message; other positions silent.
- coders/main.h: prototype in CLI section.
- coders/Makefile: `src/cli/print_arg_error.c` added to SRC.
- tests/unit/test_print_arg_error.c (new): positions 1..7 (min via get_arg_minimum), 8, and 0/9/-1 silent.

## Results
- `make verify`: norminette OK, build clean with no relink, unit tests passed.
- `make check`: green (Harness invariants OK, sealed oracles intact, src scope within declared files).
- `norminette coders/`: no errors.
- `build/tests/acceptance/test_2.4.1.bin` run directly: rc=0.
