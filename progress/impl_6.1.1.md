# Implementation 6.1.1 take_ticket

Files touched:
- coders/src/state/take_ticket.c (new; 42 header; includes limits.h locally, as other sources do)
- coders/main.h (prototype in State section; Updated line refreshed)
- coders/Makefile (src/state/take_ticket.c in SRC; Updated line refreshed)
- tests/unit/test_take_ticket.c (new; no static helper named like a phase-06 public function: only `build`, `check_unlocked`)

Design: NULL checks before any lock; under ticket_mutex only: read, bounds check
(0 <= next_ticket < LONG_MAX), increment, local copy; unlock; `*out` written only
after a successful unlock and a successful take.

Results (run sequentially):
- make check: all sections OK, norminette OK, tests passed (acceptance 6.1.1 and unit), 53 seals intact.
- make tsan: "[OK] tsan clean"
- make memcheck: "[OK] valgrind clean"
- norminette coders/: every file OK (no error lines).
