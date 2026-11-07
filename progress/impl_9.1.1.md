# Impl 9.1.1 Failure fields

Files touched:
- coders/main.h: added `int failed;` and `pthread_t monitor;` to t_sim (after `stopped`); Updated header refreshed.
- coders/src/init/init_simulation.c: `sim->failed = 0;` after `stopped = 0`; Updated header refreshed.
- tests/unit/test_init_simulation.c: dirty-storage prefill (0x5A) and `failed == 0` assertion.

`monitor` is not written by init_simulation; free_sim untouched.

Results:
- `make verify`: rc=0; norminette OK, compiles clean (-Wall -Wextra -Werror -pthread), "tests passed".
- `make check`: rc=0; "Harness invariants OK", sealed acceptance tests intact (71 seals).
- Acceptance tests/acceptance/test_9.1.1.c compiled by hand against coders/src/*.c with -Wall -Wextra -Werror: exit 0.
