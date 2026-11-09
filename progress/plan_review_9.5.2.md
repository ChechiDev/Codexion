# Plan review — subtask 9.5.2

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: `main` is the interface boundary and delegates the simulation lifecycle to the already approved `run_simulation`; the proposed stack storage and immediate return on runtime failure follow its ownership contract. Argument validation remains at the boundary.
- Scope: one entry-point change, with the exit-code macro in `coders/main.h` and the required user-facing README update at close. The three declared files match the project's living-file convention. Dependencies 9.4.1 and 9.5.1 are done.
- Test strategy: seven black-box binary tests exercise the public executable through a shell-free child process, including output, exit status, timing, invalid input and full-run leak checks. Each criterion has one test. The new oracle is red against the current validation-only `main` (AC1 produces no simulation output); it was not executed because this sandbox cannot run the suite. The acceptance file passed a warning-clean C syntax check.

## Acceptance tests (traceability)
- AC1 → `test_ac1_target_met` (red)
- AC2 → `test_ac2_burnout` (red)
- AC3 → `test_ac3_one_coder` (red)
- AC4 → `test_ac4_zero_required` (red as part of the new oracle)
- AC5 → `test_ac5_runtime_failure` (red)
- AC6 → `test_ac6_argument_errors` (existing behavior; included as regression)
- AC7 → `test_ac7_valgrind_full_runs` (red as part of the new oracle)
