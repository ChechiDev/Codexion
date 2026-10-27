# Impl 2.2.11 - Use is digit

File touched: coders/src/parsing/parse_number.c
- `slice.start[i] < '0' || slice.start[i] > '9'` replaced by `!is_digit(slice.start[i])`.
- `Updated:` header line refreshed via :Stdheader on a scratch copy (2026/10/27 19:00:51).
- No unit test added: '/' and ':' boundaries already covered in tests/unit/test_is_digit.c.

make verify: norminette OK, cppcheck SKIP (not installed), compiles clean
(-Wall -Wextra -Werror -pthread), no relink, tests passed, rc=0.
make check: all sections OK, "Harness invariants OK", rc=0.
