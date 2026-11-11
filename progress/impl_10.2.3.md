# Impl 10.2.3 - Literal errors with write

## Files touched
- coders/src/cli/print_usage.c: fixed text via `write(2, text, strlen(text))`; includes `<string.h>`, `<unistd.h>`; `<stdio.h>` removed.
- coders/src/cli/print_run_error.c: same.
- coders/src/cli/print_arg_error.c: only position 8 moved to `write`; positions 1..7 keep `fprintf`; `<stdio.h>` kept, `<string.h>`, `<unistd.h>` added.
- `Updated:` header lines refreshed from the Stdheader plugin output (2026/11/10 12:58:18). main.h untouched.
- All functions stay `void`; the write result is checked with `if (write(...) < 0) return ;` only to satisfy warn_unused_result under -Werror (no behaviour change).
- No unit test added: the sealed oracle covers exact bytes of each function; no new logic.

## Results
- `make verify`: rc=0 (norminette OK, compile clean, no relink, tests passed). `nm -u coders/codexion | grep -E "fwrite|fputs|fputc|puts"`: no match.
- Acceptance bins with CODEXION_BIN=coders/codexion: test_10.2.3, test_2.2.1, test_2.4.1, test_9.5.1 all rc=0.
- Valgrind: `CODEXION_UNDER_VALGRIND=1 valgrind -q --trace-children=yes --error-exitcode=42 build/tests/acceptance/test_10.2.3.bin`
  - Without `--log-file`: rc=134, AC5 assertion fails. Cause: AC5 closes fd 2 before exec of the child, and a child valgrind with closed stderr exits 127 (verified: `valgrind -q ./coders/codexion 1 2 2>&-` gives 127). Valgrind limitation, not the code.
  - With `--log-file=$d/vg.%p`: rc=0, no valgrind errors reported.
- TSan: out-of-tree `cc -g -fsanitize=thread ... -o $(mktemp -d)/codexion` as CODEXION_BIN, `TSAN_OPTIONS=exitcode=66 setarch -R`: rc=0.

## Conclusion
Implementation complete; all gates green. Pending Codex review.
