# impl 10.2.2 - Observable rules (test-only verification)

No files modified (spec `files:` is empty). Nothing under coders/, src/, include/, tests/acceptance/ touched.

## 1. make verify
`make verify` -> exit status 0. Ends with `[OK] norminette`, `[OK] compiles clean (-Wall -Wextra -Werror -pthread), no relink`, `[OK] tests passed` (cppcheck skipped: not installed).

## 2. Valgrind
First attempt without the env flag:
`bash -c "valgrind --trace-children=yes --error-exitcode=42 --leak-check=full build/tests/acceptance/test_10.2.2.bin"` -> rc 134 (SIGABRT):
the oracle asserts `run.err_len == 0` and valgrind's own stderr banner from the traced child makes it non-empty. Test-harness artifact, not a leak/error (ERROR SUMMARY 0, no leaks).
Same convention as 10.2.1/10.1.2 (oracle supports `CODEXION_UNDER_VALGRIND`, which relaxes alarm):
`CODEXION_UNDER_VALGRIND=1 CODEXION_BIN=$PWD/coders/codexion bash -c "valgrind --trace-children=yes --error-exitcode=42 --leak-check=full -q build/tests/acceptance/test_10.2.2.bin"` -> exit 0, no output (no errors, no leaks).

## 3. TSan
Out-of-tree build (copy of coders/Makefile, main.c, main.h, src/ into the scratchpad `tsan/` dir):
`make CFLAGS="-Wall -Wextra -Werror -pthread -I. -fsanitize=thread -g -O1" LDFLAGS="-fsanitize=thread"` -> build rc 0.
`CODEXION_BIN=<scratch>/tsan/codexion TSAN_OPTIONS=exitcode=66 setarch -R build/tests/acceptance/test_10.2.2.bin` -> exit 0, 0 "WARNING: ThreadSanitizer" reports. Shared build/ untouched by TSan objects.

## Conclusion
Oracle green, make verify green, valgrind clean, TSan clean. No production change needed. No blockers.

## Re-verification after oracle re-seal (2026-11-10)
Oracle tests/acceptance/test_10.2.2.c re-sealed (mtime 12:24); acceptance binary rebuilt by `make verify` (mtime 12:31). No production change.
- `make verify` -> exit 0 (`[OK] tests passed`). (A first attempt wrapped in `timeout 115` was killed by my wrapper, rc 124; rerun without the cap finished rc 0.)
- Valgrind: `CODEXION_UNDER_VALGRIND=1 CODEXION_BIN=$PWD/coders/codexion bash -c "valgrind --trace-children=yes --error-exitcode=42 --leak-check=full -q build/tests/acceptance/test_10.2.2.bin"` -> exit 0, no output.
- TSan (same out-of-tree TSan-built child, sources unchanged): `CODEXION_BIN=<scratch>/tsan/codexion TSAN_OPTIONS=exitcode=66 setarch -R build/tests/acceptance/test_10.2.2.bin` -> exit 0, 0 ThreadSanitizer warnings.
Conclusion: all green, no blockers.
