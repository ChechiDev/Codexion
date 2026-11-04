# Code review — subtask 6.2.2

**Verdict:** APPROVED

## Checkpoints

- C0: [x] `coders/src/time/get_deadline.c:13-33` uses the subject's permitted clock call, has no globals, and the supplied check reports Norm green.
- C1: [x] The supplied outside-sandbox `make check` exited 0; its log confirms the base files, toolchain, plan sync, and harness invariants. Tests were not run in this sandbox.
- C2: [x] The check reports consistent gates and the plan index in sync; 6.2.2 is the sole active leaf and has `PLAN_APPROVED`. `progress/current.md` still describes the preceding handoff; close-time tracking should update it.
- C3: [x] `coders/src/time/get_deadline.c:19-33` is a single-purpose time adapter. Its public declaration is in the project's single header at `coders/main.h:126`; it adds no global state or dependency cycle.
- C4: [x] The supplied check reports a clean build and tests. The three changed production files match the spec (`coders/src/time/get_deadline.c`, `coders/main.h`, `coders/Makefile`). `coders/src/time/get_deadline.c:25-32` validates first, checks addition overflow, and writes `out` only on success. There is no allocation, buffer, secret, output, or duplicated helper logic.
- C5: [x] `tests/acceptance/test_6.2.2.c:57-153` has one public-API test for each AC1–AC6 and the supplied check reports it passing. `tests/unit/test_get_deadline.c:10-45` exercises invalid input, normalization, varied delays, and the large boundary. The implementation note records clean TSan and Valgrind runs; the supplied check independently covers the acceptance oracle.
- C6: [ ] Close-stage work remains: reference document, `done` status, history, and single commit. This is expected at the code gate.

## Findings

- No blocking findings. `coders/src/time/get_deadline.c:27-32` computes the subsecond carry before the guarded seconds addition and returns a normalized absolute deadline.
