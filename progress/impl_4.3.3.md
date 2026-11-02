# Implementation 4.3.3 Pop request

Files: coders/src/heap/pop_request.c (new, static sift_down), coders/main.h
(prototype after get_head_request, Updated refreshed), coders/Makefile
(src/heap/pop_request.c in SRC, Updated refreshed), tests/unit/test_pop_request.c (new).

42 header: generated with official :Stdheader (vim -u NONE, 42header plugin) on
scratch copies with the same file names (new file: empty file; main.h/Makefile:
copies refreshed by the plugin); lines pasted with Write/Edit. Nothing hand-typed.

Inner TDD: unit test first failed to build (implicit declaration of pop_request), then green.

Results: make verify OK (norminette OK, compiles clean, tests passed),
make check OK (harness invariants OK, 41 seals intact), norminette coders/ clean,
valgrind on test_4.3.3.bin exit 0.
