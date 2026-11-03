# Impl 6.2.1 is_dongle_ready

Files touched:
- coders/src/policy/is_dongle_ready.c (new)
- coders/main.h (prototype in Policy section, Updated: refreshed)
- coders/Makefile (SRC entry, Updated: refreshed)
- tests/unit/test_is_dongle_ready.c (new, inner TDD, written first)

Design: invalid input (NULL, coder < 1, now < 0, cooldown < 0, released_at < -1)
returns 0 before the heap is read; head via get_head_request, compared by
coder; available must be 1; released_at == -1 is ready; otherwise
now >= released_at then now - released_at >= cooldown (no overflow). Pure,
read-only, returns 0/1.

Results (run sequentially):
- make check: rc=0, "Harness invariants OK", sealed oracles intact (55 seals),
  src/ within declared scope.
- make tsan: rc=0, "[OK] tsan clean".
- make memcheck: rc=0, "[OK] valgrind clean".
- norminette coders/: every file OK, no errors.
