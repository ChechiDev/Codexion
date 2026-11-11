# Implementation 10.2.1 - Viability boundary (test-only verification)

## Production change
None. No file under coders/, include/, tests/acceptance/ or README.md was
touched; `files:` is empty (AGENTS.md section 1 exception). No new unit test.

## make verify
Run alone (`pgrep -a make` empty beforehand). Output tail:

    [OK]    norminette
    [SKIP]  cppcheck not installed
    make[1]: Nothing to be done for 'all'.
    [OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
    ...
    test_main_usage: OK
    [OK]    tests passed

Exit status 0 (the task wrapper reported "exited with code 0").

## Valgrind (acceptance binary, child codexion followed)
    CODEXION_UNDER_VALGRIND=1 CODEXION_BIN=$PWD/coders/codexion \
      valgrind --trace-children=yes --error-exitcode=42 --leak-check=full -q \
      build/tests/acceptance/test_10.2.1.bin
Exit status 0, no valgrind output (no errors, no leaks), all 5 scenarios passed.

## TSan (built under bash)
    cc -g -O1 -pthread -fsanitize=thread -Icoders coders/main.c coders/src/*/*.c -o <scratch>/codexion_tsan
    cc -g -O1 -pthread -fsanitize=thread tests/acceptance/test_10.2.1.c -o <scratch>/t_tsan
    TSAN_OPTIONS=exitcode=66 CODEXION_BIN=<scratch>/codexion_tsan setarch -R <scratch>/t_tsan
Exit status 0, zero "WARNING" lines in the log (TSan-instrumented child, 120 s alarm
mode active via TSAN_OPTIONS). Also ran the regular acceptance binary with
TSAN_OPTIONS set (longer alarm) and uninstrumented codexion under setarch -R: exit 0.

## Result
All green; no blockers, no ASLR or valgrind timing issues observed.
