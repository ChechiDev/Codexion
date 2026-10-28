# Impl 2.3.2 Parse mode

Files touched:
- coders/src/parsing/parse_mode.c (new; strcmp whole-string, NULL-safe, *out untouched on failure)
- coders/main.h (prototype in Parsing section)
- coders/Makefile (SRC += src/parsing/parse_mode.c)
- tests/unit/test_parse_mode.c (new unit test)

Results:
- make verify: norminette OK, compiles clean, "[OK] tests passed" (acceptance test_2.3.2 + unit green)
- make check: rc=0, "Harness invariants OK"
- norminette coders/: no errors
