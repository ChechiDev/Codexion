# Plan review — subtask 10.1.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: the change is confined to the existing coder orchestration routine. `sleep_start_turn` owns the offset; `run_coder` only calls it and routes stop or error through its existing exit path. FIFO/EDF arbitration and the core remain untouched.
- Scope: one routine, `run_coder`, with two declared files: its production source and the living README updated by Codex at close. Dependency 10.1.1 is done. The spec remains within the approved phase scope.
- Test strategy: direct calls observe ticket allocation, silence, and failure state for even and odd seats without assuming thread order. A binary run checks the one-coder log and burnout deadline. Empirical parallel-start runs are explicitly separate from the oracle. The supplied pre-session `make check` exited 0; no tests were run in this review. The acceptance file passed a syntax-only compile.

## Acceptance tests (traceability)
- AC1 → `test_ac1_even_stops_before_request` (red: the current even coder requests a ticket before checking stop)
- AC2 → `test_ac2_odd_reaches_request` (expected green: odd seats keep their existing first request)
- AC3 → `test_ac3_offset_error_routes_failure` (red: the current even coder does not route an offset clock error before taking a ticket)
- AC4 → `test_ac4_one_coder_burnout` (regression oracle; expected green)
