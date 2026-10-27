# Impl 2.2.1 Move print usage

Files touched:
- coders/src/cli/print_usage.c (new; 42 header via :Stdheader; print_usage moved verbatim; includes stdio.h, main.h)
- coders/main.c (only `main` remains; includes stdlib.h + main.h; Updated header refreshed)
- coders/Makefile (src/cli/print_usage.c added to SRC; Updated header refreshed)
- coders/main.h and tests untouched. tests/unit/test_main_usage.c still `#include "main.c"` and passes unchanged (print_usage now resolved from the linked print_usage.o).

Results:
- `make verify`: [OK] norminette; cppcheck SKIP (not installed); [OK] compiles clean (-Wall -Wextra -Werror -pthread), no relink; `test_main_usage: OK`; [OK] tests passed.
- `make check`: all sections OK ("Harness invariants OK", src/ scope within declared files, seals intact).
- Acceptance tests/acceptance/test_2.2.1.c built manually against coders/src/cli/print_usage.c (no main.c): exit 0 (AC1 wrong counts 0..9 except 8 give identical usage/exit 1; AC2 print_usage linked without main.c; AC3 Makefile lists the file, second make "Nothing to be done", norminette no Error).
- `norminette coders`: print_usage.c, parse_number.c, trim_span.c OK.
