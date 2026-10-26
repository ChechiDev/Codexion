# Plan review — subtask 1.2.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: the proposed change is confined to the configuration types in `coders/main.h`; it adds no platform dependency. The command-line argument remains `scheduler`, as required by the subject.
- Scope: one atomic API rename in one declared production file. The phase is approved and 1.2.1 is complete.
- Test strategy: the new oracle checks the public header's types and values, verifies each removed name with an independent translation unit, and checks Norm. The prior sealed oracle was changed only at references to the renamed API. Both oracles are intentionally red against the current header; the human must re-seal `tests/acceptance/test_1.2.1.c` before implementation, as the spec requires. No tests were run in this sandbox; the supplied session-start `make check` exited 0.

## Acceptance tests (traceability)
- AC1 → `test_ac1_config_mode_field` (red: current header lacks `t_mode` and `mode`)
- AC2 → `test_ac2_distinct_modes` (red: current header lacks `MODE_FIFO` and `MODE_EDF`)
- AC3 → `test_ac3_old_names_absent` (red: current header still declares the old names)
- AC4 → `test_ac4_norminette_clean` (awaits renamed header)
