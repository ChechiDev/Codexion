# Implementation 2.4.2

Files touched:
- coders/main.c: static print_arg_errors(mask) walks bits 1..8 calling print_arg_error; main calls parse_config(argv, &config) (parse_config indexes argv[1..8]), prints errors then usage once, returns EXIT_USAGE. argc != 9 behaviour kept.
- tests/unit/test_main_usage.c: the exact-count fixture used "x" arguments, now invalid; it uses valid values (4 800 200 200 200 3 50 edf) so main returns 0.

Results: make verify green, make check green, norminette clean,
CODEXION_BIN=coders/codexion build/tests/acceptance/test_2.4.2.bin exits 0 (includes valgrind).
