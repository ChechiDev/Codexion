# Impl ISS-14.1

Files touched:
- coders/src/parsing/parse_config.c (new signature, writes invalid[1..8], returns count; Updated header refreshed)
- coders/main.h (prototype `int parse_config(char **argv, t_config *config, int *invalid);`; header refreshed)
- coders/main.c (stack `int invalid[9]`, `print_arg_errors(const int *invalid)`; header refreshed)
- tests/unit/test_parse_config.c (rewritten for new signature, inner TDD)

Result: `norminette coders/` OK; `make verify` and `make check` green (norminette OK, compile clean, tests passed, seals intact, harness invariants OK). cppcheck not installed (skipped). No bitwise operators on positions remain.
