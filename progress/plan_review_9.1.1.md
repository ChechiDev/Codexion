# Plan review — subtask 9.1.1

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers / scope: The two new `t_sim` members and clearing `failed` in `init_simulation` belong to simulation initialization in the adapters layer. The work is one state contract across the two declared production files. It adds no global or new dependency.
- Test strategy: The corrected `released_at == -1` criterion agrees with the sealed 5.4.2 oracle. The four acceptance tests use the public `main.h` API and check valid initialization from dirty storage, member types and monitor-handle use, and unchanged invalid-argument results. Static C11 member checks leave the oracle red until both fields are added. Internal failure ordering and the untouched monitor value remain source-inspection checks, as specified.
- Harness: The supplied outside-sandbox `make check` exited 0 before these acceptance tests were written. No tests or checks were run in this sandbox.

## Acceptance tests (traceability)
- AC1 → `test_ac1_four_coders_on_dirty_storage` (red)
- AC2 → `test_ac2_one_coder_on_dirty_storage` (red)
- AC3 → `test_ac3_member_types_and_monitor_handle` (red; C11 static checks also apply)
- AC4 → `test_ac4_invalid_arguments` (existing behavior)

