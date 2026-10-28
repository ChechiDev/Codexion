# Impl 3.2.1 Is stopped

Files:
- coders/src/state/is_stopped.c (new, 42 header via Stdheader): lock failure or unlock failure returns 1; else `stopped != 0`.
- coders/main.h: `# include <pthread.h>` after guard, minimal `t_sim`, `/* ---- State ---- */` with `is_stopped` prototype between Time and CLI.
- coders/Makefile: `src/state/is_stopped.c` added to SRC.
- tests/unit/test_is_stopped.c (new, inner TDD).

Results: `make verify` exit 0 (norminette OK, build clean, unit tests passed);
`make check` exit 0 (all [OK], 25 seals intact); `norminette coders/` OK;
build/tests/acceptance/test_3.2.1.bin and test_2.2.3.bin exit 0 when run directly.
cppcheck not installed (skipped).
