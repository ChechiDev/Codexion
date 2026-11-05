# Code review — subtask 8.1.1

**Verdict:** APPROVED

## Checkpoints
- C0: [x] `coders/src/monitor/get_burned_out.c:21-36` follows the subject's burnout deadline, uses permitted pthread operations, has no global state or logging; supplied harness log reports Norm green.
- C1: [x] Supplied outside-sandbox `make check` exited 0 and reports the base files, plan sync, build, tests, and harness invariants green. `make status` identifies 8.1.1 as the sole active subtask.
- C2: [x] `progress/plan_review_8.1.1.md` is `PLAN_APPROVED`; the supplied check reports all completed subtasks have approved reviews and the generated plan index is in sync. `progress/current.md` identifies phase 08 and 8.1.1 as active.
- C3: [x] `coders/src/monitor/get_burned_out.c:13-36` is one orchestration function using the existing time adapter and coder mutex through the project's single `main.h`; no new include cycle, global, or misplaced core logic.
- C4: [x] The supplied check reports warning-clean build and tests. The change is limited to the three declared production files (`coders/src/monitor/get_burned_out.c`, `coders/main.h`, `coders/Makefile`) plus its unit test. `get_burned_out.c:21-35` validates pointers, handles lock and clock errors, avoids overflow with `last <= now - time_to_burnout`, and leaves the output unchanged on failure. No allocation, buffer operation, secret, or duplicate deadline logic was introduced.
- C5: [x] `tests/acceptance/test_8.1.1.c:83-291` has one public-API test for each AC1–AC7; the supplied check reports tests passed. `tests/unit/test_get_burned_out.c:23-84` checks boundaries, extreme starts, real clock, invalid inputs, and unlocked mutex. The implementation report records clean Valgrind and ThreadSanitizer runs on the acceptance binary; those runs were not repeated in this sandbox.
- C6: [ ] Close-stage documentation, `done` status, history entry, and single commit are pending; this is the code gate, before close.

## Findings
No blocking findings. The coder mutex encloses both state and clock reads (`coders/src/monitor/get_burned_out.c:23-26`); all error paths preserve `*burned` (`coders/src/monitor/get_burned_out.c:21-35`). The new prototype and source entry are present in `coders/main.h:198-200` and `coders/Makefile:50`.
