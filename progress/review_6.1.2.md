# Code review — subtask 6.1.2

**Verdict:** APPROVED

## Checkpoints
- C0: [x] `build_request` uses authorized operations, no globals or I/O, and the supplied check reports Norm clean.
- C1: [x] The required harness files exist; the supplied outside-sandbox `make check` exited 0 (`/tmp/harness-check-code.DtsjrQ`).
- C2: [x] One leaf is in progress, its plan gate is approved, and the supplied check confirms plan sync and gate consistency.
- C3: [x] `coders/src/dongle/build_request.c:25` orchestrates existing ticket and policy APIs; coder state is read under its mutex and that mutex is released before ticket acquisition. The API is declared in `coders/main.h:181`.
- C4: [x] The implementation handles invalid input and lock failures, preserves the caller's output on failure, and touches only the three declared production files. The unit fixture now destroys both mutexes before reinitialization and at exit (`tests/unit/test_build_request.c:46`, `tests/unit/test_build_request.c:65`). No duplicated ticket or key logic was found.
- C5: [x] The seven acceptance tests cover AC1–AC7 through the public API and pass in the supplied check. Unit assertions exercise FIFO, EDF, output preservation, and unlocked state. The implementation log reports clean ThreadSanitizer and Valgrind runs; these were not rerun in the sandbox.
- C6: [ ] Close documentation, done status, history, and the single commit belong to the later close gate.

## Findings
- No blocking findings.
