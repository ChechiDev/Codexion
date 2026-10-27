# Implementation 2.2.5 - Use trim slice

Files touched: coders/src/parsing/parse_number.c (t_span span -> t_slice slice,
trim_span -> trim_slice, all uses renamed; Updated header line regenerated with
:Stdheader on a scratch copy). No other behaviour change. No new unit test needed
(existing test_parse_number.c covers the contract).

Check: grep for t_span/trim_span in parse_number.c returns 0.
make verify and make check: all green (norminette, build, unit, acceptance, harness invariants).
