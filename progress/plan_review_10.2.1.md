# Plan review — subtask 10.2.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: test-only verification exercises the delivered binary and changes no production layer.
- Scope: the empty `files:` list is valid under the declared test-only exception. The subtask checks one viability boundary claim with four exact-bound cases and one above-bound case; the sixth criterion checks the logs of those same runs.
- Subject and arithmetic: the four boundary values match `C + R + N * (C + D) + 10`, including `N = 6`, and the above-bound case exceeds its corresponding threshold. These finite runs are evidence under the scheduling assumptions in `PLAN.md`, not a proof of the guarantee or of ticket order.
- Test strategy: `tests/acceptance/test_10.2.1.c` launches the full binary, checks exit status, burnout absence and each coder's compile count, then validates complete, ordered log lines and empty stderr. The AC6 oracle now retains both captured byte counts, requires zero stderr bytes, and rejects any NUL within the captured stdout before parsing lines. This resolves the prior code-review finding without changing an acceptance criterion. The oracles may already pass at this plan gate under the test-only exception. The supplied outside-sandbox pre-session `make check` exited 0; tests were not run here. Syntax-only compilation passed with `-Wall -Wextra -Werror -pedantic`.

## Acceptance tests (traceability)
- AC1 → `test_ac1_six_coders_at_bound` (regression oracle; may be green)
- AC2 → `test_ac2_five_coders_at_bound` (regression oracle; may be green)
- AC3 → `test_ac3_cooldown_at_bound` (regression oracle; may be green)
- AC4 → `test_ac4_smallest_table_at_bound` (regression oracle; may be green)
- AC5 → `test_ac5_above_bound` (regression oracle; may be green)
- AC6 → `test_ac6_complete_ordered_logs` (regression oracle; may be green)
