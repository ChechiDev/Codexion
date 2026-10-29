# Impl 3.4.3 print_state

Files touched:
- coders/src/log/print_state.c (new, official header via Stdheader; helpers get_state_text, read_stopped, write_line)
- coders/main.h (t_state enum in Types, print_state prototype in Log, Updated line refreshed via Stdheader)
- coders/Makefile (src/log/print_state.c in SRC, Updated line refreshed via Stdheader)
- tests/unit/test_print_state.c (new unit tests)

Results:
- make verify: norminette OK, compile clean, tests passed.
- make check: all sections OK, "Harness invariants OK".
- norminette coders/: no errors.
- build/tests/acceptance/test_3.4.3.bin: rc=0, also with CODEXION_TIME_SLACK_MS=40 (rc=0).
