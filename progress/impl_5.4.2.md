# Impl 5.4.2 (revised: init_simulation)

## Changes
- Created coders/src/init/init_simulation.c (`int init_simulation(t_sim *, const t_config *)`, same static helper `init_mutexes` and behaviour as before; 42 header renamed).
- coders/main.h: prototype `init_sim` -> `init_simulation`; Updated line refreshed.
- coders/Makefile: SRC uses src/init/init_simulation.c; Updated line refreshed.
- tests/unit/test_init_simulation.c created (calls init_simulation); old tests/unit/test_init_sim.c deleted.
- setup_sim renames in test_print_state.c / test_print_burnout.c / test_sleep_ms.c kept.

## Manual deletion needed
- coders/src/init/init_sim.c (stale, not edited; no longer in SRC but the harness flags it and `find` picks it up in SRC via wildcard).
- Possibly stale build/obj/coders/src/init/init_sim.o and build/obj/coders/src/utils/trim_span.o (leftover objects; `make clean` fixes).

## Results
- make check: norminette OK, build OK, test_main_usage OK, then FAIL tests/acceptance/test_5.4.2.c (build: implicit declaration of init_sim, expected until re-seal). Integrity: FAIL "coders/src/init/init_sim.c changed outside declared files" (expected); phase 05 not PHASE_APPROVED / spec changed after approval (expected, plan gate pending).
- make test stops at the acceptance build failure, so I ran every tests/unit/test_*.c manually (linking current objects): all pass, including test_init_simulation.
- make tsan: OK (tsan clean).
- make memcheck: OK (valgrind clean).
