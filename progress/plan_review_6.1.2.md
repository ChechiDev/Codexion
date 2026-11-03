# Plan review — subtask 6.1.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `build_request` combines the existing synchronized ticket adapter and pure request-key policy in the `dongle/` orchestration module. It reads coder state under that coder's mutex, releases it before taking a ticket, and does not touch dongle state.
- Scope: one public function with three declared production files (`build_request.c`, `main.h`, `Makefile`). Its dependency 6.1.1 is done. The contract preserves the output on every failure and permits a ticket gap after a key failure, consistent with strict increase.
- Test strategy: `tests/acceptance/test_6.1.2.c` exercises the public API with one test per criterion. It covers FIFO and EDF values, untouched heaps and coder state, output preservation, exhaustion, concurrent uniqueness, and invalid inputs. The tests are left red pending the new prototype and implementation. The supplied outside-sandbox `make check` exited 0 before this oracle was added; no tests were run in this review.

## Acceptance tests (traceability)
- AC1 → `test_ac1_fifo_requests` (red)
- AC2 → `test_ac2_edf_requests` (red)
- AC3 → `test_ac3_no_queue_or_coder_mutation` (red)
- AC4 → `test_ac4_reject_invalid_edf_key` (red)
- AC5 → `test_ac5_reject_exhausted_tickets` (red)
- AC6 → `test_ac6_concurrent_requests` (red)
- AC7 → `test_ac7_reject_null_inputs` (red)
