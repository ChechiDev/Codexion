# Impl 4.3.2 Get head request

Files touched:
- coders/src/heap/get_head_request.c (new)
- coders/main.h (prototype after push_request; Updated header refreshed)
- coders/Makefile (SRC entry; Updated header refreshed)
- tests/unit/test_get_head_request.c (new unit test, written first)

Results (real):
- make verify: norminette OK, compiles clean, tests passed (cppcheck not installed, SKIP).
- make check: all sections OK, integrity OK, "Harness invariants OK".
- norminette coders/: no errors.
- valgrind on build/tests/acceptance/test_4.3.2.bin: exit 0.

Header method: the `Updated:` lines of coders/main.h and coders/Makefile, and the
full header of coders/src/heap/get_head_request.c, were generated with the official
:Stdheader plugin (vim -u NONE -N -es ... -c 'Stdheader' -c 'wq', per
docs/conventions.md) on scratch copies with the same file names, then pasted with
Edit. No hand-written timestamps.

Deviation: none.
