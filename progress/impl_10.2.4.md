# Implementation 10.2.4 Evaluation runs

## Files touched
- Makefile (dev-only root): SCENARIOS gains `5 800 200 200 200 3 0 edf` and `6 870 100 50 50 3 10 edf`; existing entries kept.

## Verification (in progress)
- `make verify` (first run): failed in test_3.4.6 while the stop hook ran `make check` concurrently on build/; waited for `pgrep -x make` empty and reran: exit 0, "[OK] tests passed", norminette OK.
- Oracle under `valgrind --trace-children=yes --error-exitcode=42 -q` with CODEXION_UNDER_VALGRIND=1 (bash): rc 134. Without slack AC3 fails (valgrind slows timing, ts out of 410..420); with CODEXION_TIME_SLACK_MS=1000 (oracle max) AC5 fails at `result.status == 0`: nesting issue, the oracle itself launches valgrind inside a valgrind, which the outer one does not support cleanly. Not forced; the oracle is green unnested in make verify, and valgrind is applied directly to the binary below.
- leader `make memcheck` (foreground, alone, 7 SCENARIOS incl. the two new ones): exit 0, "[OK] valgrind clean".
- leader `make tsan` (foreground, alone): exit 0, "[OK] tsan clean".
- leader empirical: `5 800 200 200 200 3 0 edf` x10 -> 1 burnout(s)
- leader empirical: `4 800 200 200 200 3 0 edf` x10 -> 0 burnout(s)
- leader empirical: `4 610 200 200 200 3 0 edf` x10 -> 1 burnout(s)
- leader empirical: `5 650 200 200 200 3 0 edf` x10 -> 0 burnout(s)
- leader empirical: `4 410 200 200 0 3 0 edf` x10 -> 1 burnout(s)
- User decision (2026-11-10): the 1/10 burnouts above are accepted as below the PLAN.md sufficient bound; documented for the defense, no investigation.
