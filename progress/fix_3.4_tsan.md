# Fix 3.4 TSan (test_print_burnout.c)

Changed only tests/unit/test_print_burnout.c: removed the case that locked
stop_mutex before print_burnout (lock-order inversion; covered by acceptance
AC10), renamed test to test_bad_coder, added destroy_sim() called at the end
of each test (no re-init of a live t_sim).

Verification: make check exit 0; make tsan "[OK] tsan clean";
make memcheck "[OK] valgrind clean" (valgrind installed).
