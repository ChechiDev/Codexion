# Code review — subtask 4.1.2

**Verdict:** APPROVED

## Checkpoints
- C0: [x] The FIFO ticket and EDF deadline follow the subject; the function uses no forbidden facility, global, output, or shared state. The supplied `make check` log reports Norm clean.
- C1: [x] The supplied outside-sandbox `make check` exited 0; its log confirms the base files, toolchain, plan sync, and harness invariants.
- C2: [x] The plan has one in-progress leaf, 4.1.2; its plan gate is `PLAN_APPROVED`, and the harness reports gate consistency. `progress/current.md` still describes the prior close and should be refreshed at close.
- C3: [x] `get_request_key.c` is pure core policy arithmetic; `main.h` is the project's single public header, and no dependency or global state was added.
- C4: [x] The supplied check includes a green `make verify`. The function has one responsibility, validates inputs, checks overflow before addition, and needs no allocation or buffer. The three production files match the spec.
- C5: [x] `test_4.1.2.c` has one public-API test per AC1–AC6; the unit test exercises FIFO, EDF, invalid input, and the overflow boundary. The supplied check reports tests passed; the implementation report records clean TSan and Valgrind runs.
- C6: [ ] Close-stage work is pending: reference documentation, `done` status, history, and the single commit. These are not code-gate requirements.

## Findings
- `coders/src/policy/get_request_key.c:19-29`: Validation and both policies match the spec. EDF checks `LONG_MAX - time_to_burnout` before adding; FIFO returns before reading the time values.
- `coders/main.h:117-118` and `coders/Makefile:28`: The public declaration and build entry are present. Both files have refreshed 42 `Updated:` lines.
- `tests/unit/test_get_request_key.c:8-21`: Unit assertions cover meaningful boundary and error cases; acceptance tests supply full criterion traceability.

## Requested changes
None.
