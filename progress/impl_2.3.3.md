# Implementation 2.3.3 Parse config

Files touched:
- coders/src/parsing/parse_config.c (new: parse_config, static parse_field, static set_field_addresses)
- coders/main.h (parse_config prototype, Parsing section)
- coders/Makefile (src/parsing/parse_config.c in SRC)
- tests/unit/test_parse_config.c (new unit tests)

Notes: reuses parse_number, get_arg_minimum, parse_mode; no early exit; mask bit n for invalid arg n.

Results: `make verify` green (norminette OK, compiles clean, tests passed);
`make check` rc=0, "Harness invariants OK"; `norminette coders/` reports no error.

Review fix: renamed static bind_fields to set_field_addresses (allowed set_ prefix).
