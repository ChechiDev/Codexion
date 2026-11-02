# Code review — subtask 5.1.1

**Verdict:** APPROVED

## Checkpoints
- C0: [x] `coders/src/init/get_dongle_pair.c:15-36` implements the subject's circular seat mapping without globals, allocation, I/O, or prohibited calls. The supplied harness log reports Norm clean.
- C1: [x] The supplied outside-sandbox `make check` exited 0; its log confirms the base files, documentation, plan sync, gate consistency, and source scope. No tests were run in this sandbox.
- C2: [x] `make status` shows 5.1.1 as the sole in-progress leaf with `PLAN_APPROVED`; the harness confirms the generated index is in sync and completed leaves have approved reviews. `progress/current.md` names the active subtask, though its next-step wording predates the plan gate and should be refreshed at close.
- C3: [x] `coders/src/init/get_dongle_pair.c:13-36` is pure core logic with one responsibility and no platform calls or global state. Its declaration is in the project's single public header at `coders/main.h:137`.
- C4: [x] `coders/src/init/get_dongle_pair.c:19-35` validates all inputs before writing outputs, avoids overflow at `INT_MAX`, and orders the pair for later lock acquisition. No allocation or buffer is involved. The only changed production files are the three declared in the spec; the supplied harness log reports a warning-clean build, Norm clean, and passing tests. No duplicate mapping logic was found.
- C5: [x] `tests/acceptance/test_5.1.1.c` exercises the public API with one test per AC1–AC5; `tests/unit/test_get_dongle_pair.c:9-58` adds ring, boundary, and invalid-input coverage. The supplied harness log reports the tests passed. `progress/impl_5.1.1.md` reports clean ThreadSanitizer and Valgrind runs; those were not rerun here.
- C6: [ ] Close work is pending at the code gate: the reference document, `done` status, history entry, and single commit belong to the later close step.

## Findings
- No blocking findings. `coders/src/init/get_dongle_pair.c:19` preserves both outputs on invalid input; lines 21-24 avoid `seat + 1` overflow; lines 25-34 return the lower index first. `coders/Makefile:25` includes the source, and `coders/main.h:135-137` declares it in the new Init section.
