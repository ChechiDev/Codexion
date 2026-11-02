# Issue review — ISS-14

**Verdict:** ISSUE_APPROVED

## Analysis
- Coverage and size: ISS-14.1 replaces the parser's mask with flags and adapts its sole caller, covering the imported issue in one small, buildable change. The three declared production files are the parser, its prototype, and the caller; the caller edits only consume the changed contract. Parsing remains pure and user-facing diagnostics remain in the interface layer.
- Dependencies: 2.3.3 and 2.4.2 are done. The single issue subtask introduces no cycle.
- Acceptance: AC1–AC4 observe the new public C API, including flag reset, count, index zero, validation, and unchanged input. AC5 observes binary diagnostics, order, usage, exit status, and a valid run. AC6 covers build, Norm, and valgrind. All criteria name `tests/acceptance/test_ISS-14.1.c`; source-only requirements are in the `Code review check:` line.
- Migration: `tests/acceptance/test_2.3.3.c` exists and is the only sealed oracle that calls `parse_config`; it must be migrated at the plan gate from mask assertions to flag and count assertions. The sealed binary oracle `tests/acceptance/test_2.4.2.c` needs no change.
- Classification: the parser's public C API changes, so the structural-refactor exception does not apply. The spec explicitly requires red API oracles at the plan gate and preserves user-visible binary behavior as regression evidence.

## Per subtask
- ISS-14.1: ok — one parser-contract migration with the necessary caller adaptation, three declared production files, observable criteria, and a specified sealed-oracle migration.
