# Code review — subtask 2.2.1

**Verdict:** APPROVED

## Checkpoints
- C0: [x] `print_usage` uses subject-authorized `fprintf`, preserves the exact usage text, and the supplied check reports Norm clean (`coders/src/cli/print_usage.c:16-22`).
- C1: [x] Required harness files and directories exist; the supplied `/tmp/harness-check-code.DYT4dU` reports `make check` exit 0.
- C2: [x] `make status` shows one active leaf, 2.2.1, with `PLAN_APPROVED`; the supplied check reports plan sync and gate consistency. `progress/current.md` still lists the earlier plan-gate next step and needs updating at close.
- C3: [x] User-facing output is in the CLI interface module; `main.c` only wires the call (`coders/src/cli/print_usage.c:16`, `coders/main.c:16-25`). The project's single-header convention applies; this change adds no global state or layer dependency.
- C4: [x] All three changed production files are declared in the spec (`coders/src/cli/print_usage.c:16-22`, `coders/main.c:16-25`, `coders/Makefile:17-20`). The move is verbatim, with one definition and no duplicated logic. The supplied check reports a clean compile, no relink, Norm clean, and passing tests. No allocation, buffer operation, or external input handling was added.
- C5: [x] `tests/acceptance/test_2.2.1.c:75-145` has one test for each AC1–AC3 and uses the executable or public declaration. Existing unit tests exercise wrong and correct argument counts and the usage content (`tests/unit/test_main_usage.c:84-136`). The supplied check runs the tests and reports success. [ ] Optional sanitizer/Valgrind evidence was not supplied; this move adds no memory ownership.
- C6: [ ] Close tasks remain: reference note, `done` status, plan sync, history, and one commit. These belong to the later close gate, not this code review.

## Findings
None.
