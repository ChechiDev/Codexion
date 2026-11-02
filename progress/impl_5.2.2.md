# Impl 5.2.2 Destroy dongle

Files touched:
- coders/src/cleanup/destroy_dongle.c (new): NULL -> -1; free_heap, then pthread_cond_destroy, then pthread_mutex_destroy; every step runs, result -1 if either destroy failed, else 0; no lock/alloc/clock/output/global.
- coders/main.h: new `/* ---- Cleanup ---- */` section after Init with the prototype; Updated header refreshed.
- coders/Makefile: src/cleanup/destroy_dongle.c added to SRC; Updated header refreshed.
- tests/unit/test_destroy_dongle.c (new): empty and queued dongle, heap reset, available/released_at untouched, NULL -> -1.

Results (real):
- make check: first run failed only on test_3.4.5 AC2 (no-relink mtime assertion, a timing flake right after the Makefile change forced a rebuild); rerun: exit 0, all OK, harness invariants OK, norminette OK, acceptance test_5.2.2 passes.
- make tsan: exit 0, "tsan clean".
- make memcheck: exit 0, "valgrind clean".
- norminette coders/: no non-OK lines.
- valgrind on build/tests/acceptance/test_5.2.2.bin: exit 0, "All heap blocks were freed", 0 errors.
