# Code review — subtask 5.2.1

**Verdict:** APPROVED

## Checkpoints
- C0: [x] `coders/src/init/init_dongle.c:15-35` uses only subject-authorized pthread operations and the existing heap API. It adds no global state, I/O, or clock calls; the supplied harness reports Norm clean.
- C1: [x] The supplied outside-sandbox `make check` exited 0 and confirms base files, plan sync, gate consistency, source scope, compilation, and passing tests. No tests were run in this sandbox.
- C2: [x] `make status` shows 5.2.1 as the sole in-progress leaf with `PLAN_APPROVED`; the supplied harness confirms the generated index and completed gates are consistent. `progress/current.md` names 5.2.1, though its next-step wording predates the plan gate and should be refreshed at close.
- C3: [x] `coders/src/init/init_dongle.c:15-35` keeps mutex, condition, and allocation lifecycle in the adapter layer and delegates heap storage to the existing core API. The type and function are declared in the project's single public header at `coders/main.h:82-89,147`; no global or circular dependency was added.
- C4: [x] The three changed production files match the spec. `coders/src/init/init_dongle.c:17-33` validates NULL, checks each initialization result, rolls back successful steps in reverse order, and writes state fields only after success. `init_heap` checks allocation and writes heap fields only on success (`coders/src/heap/init_heap.c:22-33`). The function is short and reuses existing heap logic; no duplicated logic, buffer, external input, secret, or sensitive logging is involved. The supplied harness reports warning-clean compilation, Norm clean, and passing tests.
- C5: [x] `tests/acceptance/test_5.2.1.c:25-125` has one public-API check for each AC1–AC6, including a timed condition wait, capacity/order, and independent instances. `tests/unit/test_init_dongle.c:13-35` covers sentinel-filled storage, initialized state, mutex usability, cleanup, and NULL rejection. The supplied harness reports tests passed; `progress/impl_5.2.1.md` reports clean ThreadSanitizer and Valgrind runs, including the acceptance binary. Those runs were not repeated here.
- C6: [ ] Close work is pending by design at the code gate: the reference document, `done` status, history entry, and single commit belong to the later close step.

## Findings
- No blocking findings. `coders/src/init/init_dongle.c:19-30` follows the required mutex → condition → heap initialization order and handles all three failure points without leaving a successfully initialized resource live. `coders/Makefile:26` includes the source; both modified deliverable files have refreshed 42 headers.
