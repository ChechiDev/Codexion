# Plan review — subtask 5.2.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `init_dongle` owns pthread and heap lifecycle in the adapter layer; the heap and ordering remain in the core. The planned reverse rollback and no-live-resource failure contract fit the ownership rules and the binding subject.
- Scope: one public initializer, with three declared production files (`init_dongle.c`, `main.h`, `Makefile`). Dependencies 4.2.2 and 5.1.1 are done, and phase 05 has `PHASE_APPROVED`.
- Test strategy: six public-API acceptance checks cover successful initial state, usable synchronization primitives, capacity and ordering, independent instances, and NULL rejection. The oracle intentionally remains red until `t_dongle` and `init_dongle` are implemented. Failure injection for pthread and allocation errors is not portable here; the code gate must inspect each rollback path as specified.

## Acceptance tests (traceability)
- AC1 → `test_ac1_initial_state` (red)
- AC2 → `test_ac2_mutex_unlocked` (red)
- AC3 → `test_ac3_condition_wait` (red)
- AC4 → `test_ac4_two_neighbour_heap` (red)
- AC5 → `test_ac5_independent_dongles` (red)
- AC6 → `test_ac6_null_dongle` (red)
