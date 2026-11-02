# Review history: ISS-14

## Round 1: issue gate, CHANGES_REQUESTED (2026-11-02 10:51)

### Issue review — ISS-14

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage and size: the proposed work covers the imported issue and fits one issue. The declared dependencies 2.3.3 and 2.4.2 are done, and the listed migration target `tests/acceptance/test_2.3.3.c` exists. The binary regression oracle `tests/acceptance/test_2.4.2.c` must remain sealed and unchanged.
- Scope: the only subtask changes both the parser's public contract and the entry point's error reporting. These are two units of work across the core and interface layers; the reviewer's one-function micro-task rule requires a split. The issue header itself suggests separate parser and `main.c` subtasks (lines 60–63). Plan the transition so each completed subtask builds and preserves the required gates.
- Acceptance: AC1–AC4 describe useful observable parser cases. AC5 is a source-inspection requirement, and its `|=` text breaks the plan parser's pipe-delimited criterion format: `feature_list.json` gives AC5 a truncated criterion and a non-path `test` value (see `feature_list.json:2096–2099`). AC6 names another subtask's sealed test as its own test path, contrary to the one-test-path-per-subtask rule. The source inspection belongs in a `Code review check:` line; binary regression can be tested through this issue's acceptance path while the existing 2.4.2 oracle also remains unchanged.
- Refactor classification: the issue calls this a structural refactor (`docs/plan/issues/ISS-14-refactor-replace-parse-config-bit-mask-with-invalid-flags-2-3.md:37–40`), but the public C API changes from two parameters and mask return to three parameters and count return (`coders/main.h:94`; issue header lines 19–25). That is observable to C callers. The no-behaviour-change exception in `AGENTS.md` therefore does not apply to the parser subtask; its new API acceptance tests should be red at the plan gate. User-visible binary behavior can still be a regression requirement.

#### Per subtask
- ISS-14.1: changes requested — bundles parser and entry-point work; AC5 is malformed and checks source text; AC6 uses a different subtask's test path; structural-refactor claim conflicts with the public API change.

#### Requested changes
1. Split ISS-14.1 into atomic parser/API and entry-point subtasks, with explicit dependencies, 1–3 declared production files apiece, and a buildable transition at each gate. Assign the sealed `tests/acceptance/test_2.3.3.c` migration to the subtask that changes the parser signature. Keep `tests/acceptance/test_2.4.2.c` unchanged.
2. Rewrite AC5 without pipe characters that break the spec format. Put the required absence of bit-mask operators and the new prototype in `Code review check:` lines; keep acceptance criteria on observable behavior. Regenerate the plan index with `make plan`.
3. Give each subtask's acceptance criteria its own `tests/acceptance/test_ISS-14.<k>.c` path. Cover unchanged binary diagnostics in the entry-point subtask's oracle and retain the sealed 2.4.2 regression test as additional evidence.
4. State clearly that the parser's public API behavior changes, so its plan-gate tests must target the new signature and return contract. Reserve the structural-refactor exception for a subtask with no observable public API or binary behavior change.

## Round 2: issue gate, ISSUE_APPROVED (2026-11-02 10:56)

### Issue review — ISS-14

**Verdict:** ISSUE_APPROVED

#### Analysis
- Coverage and size: ISS-14.1 replaces the parser's mask with flags and adapts its sole caller, covering the imported issue in one small, buildable change. The three declared production files are the parser, its prototype, and the caller; the caller edits only consume the changed contract. Parsing remains pure and user-facing diagnostics remain in the interface layer.
- Dependencies: 2.3.3 and 2.4.2 are done. The single issue subtask introduces no cycle.
- Acceptance: AC1–AC4 observe the new public C API, including flag reset, count, index zero, validation, and unchanged input. AC5 observes binary diagnostics, order, usage, exit status, and a valid run. AC6 covers build, Norm, and valgrind. All criteria name `tests/acceptance/test_ISS-14.1.c`; source-only requirements are in the `Code review check:` line.
- Migration: `tests/acceptance/test_2.3.3.c` exists and is the only sealed oracle that calls `parse_config`; it must be migrated at the plan gate from mask assertions to flag and count assertions. The sealed binary oracle `tests/acceptance/test_2.4.2.c` needs no change.
- Classification: the parser's public C API changes, so the structural-refactor exception does not apply. The spec explicitly requires red API oracles at the plan gate and preserves user-visible binary behavior as regression evidence.

#### Per subtask
- ISS-14.1: ok — one parser-contract migration with the necessary caller adaptation, three declared production files, observable criteria, and a specified sealed-oracle migration.
