# Impl 4.1.2 Get request key

## Files touched
- coders/src/policy/get_request_key.c (new, 42 header)
- coders/main.h (prototype in Policy section; Updated line refreshed)
- coders/Makefile (SRC entry before is_request_before.c; Updated line refreshed)
- tests/unit/test_get_request_key.c (new unit test)

## Inner TDD
Unit test written first; compile failed with implicit declaration of
get_request_key (red). After implementing, it passes.

## Implementation notes
Order: ticket < 0 -> -1; fifo -> ticket; mode not edf -> -1; edf: start < 0 or
burnout < 1 -> -1; start > LONG_MAX - burnout -> -1 (check before add, no
saturation); return start + burnout. <limits.h> only in the .c.

## Verification (real results)
- norminette coders/: all OK (no error lines)
- make check: all sections [OK], harness invariants OK (src scope, 35 seals)
- make verify: [OK] norminette, compiles clean/no relink, [OK] tests passed
  (unit + acceptance run silently on success)
- make tsan: [OK] tsan clean
- make memcheck: [OK] valgrind clean
