# Impl 6.1.2 build_request

Files touched:
- coders/src/dongle/build_request.c (new; static helper `get_last_start` (renamed from read_last_start: `read_` not in the verb list) copies last_compile_start under the coder mutex and unlocks before take_ticket)
- coders/main.h (new `/* ---- Dongle ---- */` section after Cleanup; Updated refreshed)
- coders/Makefile (src/dongle/build_request.c in SRC; Updated refreshed)
- tests/unit/test_build_request.c (new unit test)

Results (sequential runs):
- make check: rc=0, "tests passed", "Harness invariants OK" (includes unit test_build_request.bin and sealed acceptance test_6.1.2.bin; both built and run)
- make tsan: rc=0, "tsan clean"
- make memcheck: rc=0, "valgrind clean"
- norminette coders/: all OK (build_request.c: OK!, no errors)

No commit made, status untouched.

Rename re-run (get_last_start): make check rc=0, tsan clean, valgrind clean, norminette 0 errors.

Unit test fix (destroy_fixture before re-setup and at exit): make check rc=0, tsan clean, valgrind clean, norminette 0 errors.
