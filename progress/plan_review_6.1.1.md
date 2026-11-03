# Plan review — subtask 6.1.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: the ticket counter belongs to simulation state; the mutex operation is correctly placed in an adapter. No core policy or dongle logic is bundled into this function.
- Scope: one public function in one new `.c` file, with its prototype and build entry in the other two declared files. The contract bounds the counter before incrementing and identifies the lock-order rule.
- Test strategy: `tests/acceptance/test_6.1.1.c` exercises the public API on an initialized simulation. It covers sequential allocation, boundaries, failure preservation, concurrent uniqueness and caller ordering, and null inputs. Lock and unlock failure paths are reserved for code inspection because they cannot be forced deterministically. The new oracle is expected to be red until `take_ticket` is implemented. The supplied outside-sandbox `make check` exited 0 before this review; no tests were run here.

## Acceptance tests (traceability)
- AC1 → `test_ac1_sequential_tickets` (red)
- AC2 → `test_ac2_counter_boundaries` (red)
- AC3 → `test_ac3_reject_exhausted_or_negative` (red)
- AC4 → `test_ac4_concurrent_unique_tickets` (red)
- AC5 → `test_ac5_reject_null_inputs` (red)
