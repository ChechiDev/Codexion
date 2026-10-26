# Phase review — phase 01

**Verdict:** PHASE_APPROVED

## Analysis
- **Coverage:** Tasks 1.1 and 1.2 deliver the phase goal. The four subtasks cover the deliverable build, configuration types, the owner-requested API rename, and argument-count usage. The external `scheduler` argument retains the subject's required name; only its stored C names change.
- **Scope and dependencies:** Each subtask declares one to three files. Shared files have the explicit dependency chain 1.1.1 → 1.2.1 → 1.2.2 → 1.2.3, with no cycle.
- **Acceptance:** Every subtask has observable criteria and one distinct `tests/acceptance/test_<id>.c` path. Acceptance files exist for the two completed subtasks; the pending subtasks receive theirs at their plan gates. The 1.2.2 spec explicitly schedules the older sealed oracle's API-name update and re-seal before implementation.
- **Phase discipline:** Phases 02–10 have titles and goals only.
- **External check:** The supplied `make check` exited 2 solely on phase approval and seal mismatches caused by this amendment. Its build, tests, Norm, acceptance seals, scope, and other sections report `[OK]`. For this owner-authorized re-gate, these self-referential failures are expected until `PHASE_APPROVED` re-seals the phase. No tests were run in this sandbox.

## Per subtask
- **1.1.1:** Scope and acceptance are sound. `coders/Makefile` defines the required rules and flags, `coders/main.c` supplies the stub entry point, and `README.md` has the required initial sections. `tests/acceptance/test_1.1.1.c` exists. The later argument-count references now identify 1.2.3.
- **1.2.1:** Scope and acceptance are sound. `coders/main.h` defines the sealed `t_policy` and eight-field `t_config`; `coders/Makefile` recompiles objects when the header changes. `tests/acceptance/test_1.2.1.c` exists. The approved rename in 1.2.2 supersedes these names.
- **1.2.2:** One-file API rename, dependent on 1.2.1. Criteria cover the new field and enum, absence of old names, and Norm. The older oracle migration and re-seal are explicitly scheduled at this subtask's plan gate.
- **1.2.3:** Argument-count validation and usage output form one entry-point behavior. Its three declared files cover the implementation, shared declaration, and README. Criteria cover error status and streams, required argument order, Norm, and instructions.
