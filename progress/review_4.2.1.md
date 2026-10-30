# Code review — subtask 4.2.1

**Verdict:** APPROVED

## Checkpoints
- C0: [x] The heap allocation supports the subject's required priority queue; it uses only allowed `malloc`, no globals, and the supplied check reports Norm clean.
- C1: [x] The supplied outside-sandbox `make check` exited 0. Its log confirms the base files, toolchain, plan sync, and harness invariants.
- C2: [x] 4.2.1 is the sole in-progress leaf, its plan gate is `PLAN_APPROVED`, and the harness reports gate consistency. `progress/current.md` identifies the active subtask, though its next-step text predates implementation and should be refreshed at close.
- C3: [x] `init_heap` is core allocation code with no I/O, clock, thread, or global dependency. Its type and declaration are in the project's single public header.
- C4: [x] The supplied check includes green verification. The function has one responsibility, validates arguments and multiplication bounds, leaves the out-parameter untouched on failure, and allocates once. The three production files match the spec.
- C5: [x] The acceptance oracle has one public-API test for each AC1–AC6 and passed in the supplied check. The unit test exercises success, invalid capacities, and unchanged output; the implementation report records clean TSan, Valgrind, and an explicit Valgrind run of the acceptance binary.
- C6: [ ] Close-stage reference documentation, `done` status, history, and the single commit are pending; they are not code-gate requirements.

## Findings
- `coders/src/heap/init_heap.c:22-33`: NULL and invalid capacity checks precede size calculation and allocation. On allocation failure no heap field is written; on success all three fields are set. No prior heap field is read.
- `coders/main.h:75-80`, `coders/main.h:127-129`, and `coders/Makefile:20`: The type, prototype, and build entry match the spec. The 42 `Updated:` lines were refreshed.
- `tests/unit/test_init_heap.c:9-49`: Unit assertions check writable capacity, invalid inputs, preservation on failure, and NULL output. The acceptance oracle additionally checks independent live allocations and deliverable integration.

## Requested changes
None.
