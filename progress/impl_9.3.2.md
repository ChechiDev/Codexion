# Implementation 9.3.2 start_threads

## Files touched
- coders/src/lifecycle/start_threads.c (new; static create_coders, recover_start; start_threads)
- coders/main.h (prototype in Lifecycle section after join_threads; Updated line)
- coders/Makefile (src/lifecycle/start_threads.c in SRC; Updated line)
- tests/unit/test_start_threads.c (new: NULL -> -1; 2-coder run, join, free)

Header/Updated lines were written by hand in Stdheader format (editor not available to the implementer).

## Results
- `make verify`: rc=0 (norminette OK, cppcheck not installed -> SKIP, clean build no relink, tests passed).
- `make check`: rc=0, no FAIL lines, "Harness invariants OK", sealed oracles intact (77).

## Evidence (bash, build/tsan_9.3.2)
Built with `cc -Wall -Wextra -Werror -pthread -g -Icoders tests/acceptance/test_9.3.2.c $(find coders/src -name '*.c')`.
- Native run `timeout 60 build/tsan_9.3.2/native`: exit 0, no stderr diagnostic.
- `CODEXION_UNDER_VALGRIND=1 timeout 400 valgrind --trace-children=yes --error-exitcode=42 native`: rc=0, "All heap blocks were freed", 0 errors; stderr "AC3: skipped under sanitizer or valgrind".
- Rebuild with `-fsanitize=thread`, `TSAN_OPTIONS=exitcode=66 timeout 300 setarch -R tsan`: rc=0; AC3 skipped (sanitizer).

## AC3
Native run exit 0 with no "not exercised" diagnostic on stderr: the child exited 0, i.e. the creation-failure path was exercised (start_threads returned -1, failed==1, dongles available, free_sim 0). Skipped under valgrind and TSan as designed.

## Code-gate fix: headers via Stdheader
The hand-written header/Updated lines were replaced with plugin output. Per docs/conventions.md, for each of start_threads.c, main.h, Makefile on an empty scratch file:
`vim -u NONE -N -es --cmd 'set rtp+=~/.vim/pack/42/start/42header' -c "let g:user42='sperez-l'" -c "let g:mail42='sperez-l@student.42barcelona.com'" -c 'runtime! plugin/stdheader.vim' -c 'Stdheader' -c 'wq' <scratch>/<name>`
Then the new file's Created/Updated (2026/11/08 18:08:23) and the Updated lines of main.h (18:08:23) and Makefile (18:08:24) were pasted with Edit; the header of start_threads.c was diffed identical to the plugin output.
Re-run: `make verify` rc=0; `make check` rc=0 (no FAIL, Harness invariants OK). The valgrind and TSan evidence above is kept (only comments changed).
