# Plan review — subtask 4.1.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `get_request_key` is pure policy arithmetic in `coders/src/policy/`; it needs no adapter, clock, lock, allocation, or port. FIFO uses the ticket, while EDF uses the subject's deadline on the common elapsed-time base.
- Scope: one public function is planned in three declared production files: its source, `coders/main.h`, and `coders/Makefile`. The existing comparator handles ties separately. No concerns are bundled.
- Validation: negative tickets, invalid modes, invalid EDF inputs, and signed-long overflow have explicit error results. The pre-addition bound avoids undefined signed overflow; valid keys cannot equal the -1 sentinel.
- Test strategy: the six acceptance tests below exercise only the public `main.h` API and the deliverable build, symbol, and Norm behavior. The new oracle is intentionally red because `get_request_key` is not yet declared or implemented. The supplied outside-sandbox `make check` result was exit 0 before the oracle was added; no tests were run in this review.

## Acceptance tests (traceability)
- AC1 → `test_ac1_fifo_uses_ticket` (red)
- AC2 → `test_ac2_edf_deadline_ignores_ticket` (red)
- AC3 → `test_ac3_edf_long_upper_bound` (red)
- AC4 → `test_ac4_invalid_input` (red)
- AC5 → `test_ac5_valid_keys_do_not_collide_with_error` (red)
- AC6 → `test_ac6_build_symbol_and_norm` (red)
