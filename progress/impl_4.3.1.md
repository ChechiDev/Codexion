# Impl 4.3.1 Push request

Files touched:
- coders/src/heap/push_request.c (new: `push_request`, static `sift_up`, inline swap)
- coders/main.h (prototype in Heap section; `Updated:` refreshed)
- coders/Makefile (src/heap/push_request.c in SRC; `Updated:` refreshed)
- tests/unit/test_push_request.c (new unit test, written first)

Results:
- `make verify`: norminette OK, compiles clean no relink, "tests passed".
- `make check`: harness invariants OK (final run exit 0).
- `norminette coders/`: clean.
- valgrind on build/tests/acceptance/test_4.3.1.bin: exit 0.

Note: two earlier `make check` runs showed a transient `[FAIL]` (test_3.4.5, then
test_4.3.1 at AC9 `run_command`); the tests call `make -C coders`/norminette via
system() and appear to race when run concurrently with the build. Re-runs pass,
and the binaries pass standalone (rc=0).
