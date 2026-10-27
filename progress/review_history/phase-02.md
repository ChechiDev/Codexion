# Review history: phase-02

> Reconstructed on 2026-10-27 from the session log (verdict texts verbatim;
> times not recorded).

## Round 1: phase gate, CHANGES_REQUESTED (2026-10-27)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- **Coverage and scope:** The seven subtasks cover numeric parsing, field minimums, exact scheduler parsing, configuration, error messages, and CLI validation. Each is a small unit with 1–3 declared files. Shared `coders/main.h` and `coders/Makefile` edits have explicit dependency order; dependencies are acyclic and respect the layers. Later phases remain unbroken.
- **Acceptance:** Each subtask names one acceptance test path, except 2.2.3 AC7 as parsed by the plan generator. Its literal `|` operators split the criterion, leaving an invalid `test` value in `feature_list.json`. The phase 2.2 goal also conflicts with the required multi-error behavior.

#### Per subtask
- 2.1.1: ok; one pure trim primitive, with observable span and build criteria.
- 2.1.2: ok; one numeric parser, with observable boundary and rejection criteria.
- 2.2.1: ok; one minimum lookup, shared by parsing and error output.
- 2.2.2: ok; one exact, case-sensitive scheduler parser.
- 2.2.3: change required; AC7 is misparsed, and its multi-error behavior contradicts the phase 2.2 goal.
- 2.3.1: ok; one error-line printer with observable output criteria.
- 2.3.2: ok; one entry-point integration with binary and README criteria.

#### Requested changes
1. In `docs/plan/phase-02-argument-parsing/2.2.3-parse-config.md:16`, rewrite AC7 without literal `|` characters in the criterion, then run `make plan`. Confirm `feature_list.json` records the complete criterion and `tests/acceptance/test_2.2.3.c` as AC7's test path. Currently its generated `test` field starts with `(1 << 3)` instead.
2. In `docs/plan/phase-02-argument-parsing.md:10`, change task 2.2's goal from reporting the *first* invalid position to reporting every invalid position, consistent with 2.2.3 AC7 and 2.3.2 AC3. Regenerate the plan index afterward.

## Round 2: phase gate, CHANGES_REQUESTED (2026-10-27)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- **Coverage and scope:** The three tasks cover the phase goal. Each of the seven pending subtasks has one focused function or entry-point integration and declares 1–3 production files. Phase 03 onward remains unbroken.
- **Dependencies:** The graph has no cycle and the core parsing functions precede the CLI integration. One shared-file dependency is missing: 2.3.1 and 2.2.2/2.2.3 all modify `coders/main.h` and `coders/Makefile`, but 2.3.1 is not ordered after either of the latter subtasks.
- **Acceptance:** Every criterion has an observable result or tooling outcome and names its subtask's single `tests/acceptance/test_<id>.c` path. The prior AC7 delimiter error in 2.2.3 is fixed in `feature_list.json`; task 2.2 now agrees with the multi-error criteria. The supplied session-start `make check` exited 0; tests were not run in this sandbox.

#### Per subtask
- 2.1.1: ok; one pure span-trimming primitive, with boundary and immutability criteria.
- 2.1.2: scope and acceptance ok; its description incorrectly points to 2.2.2 for minimum checks.
- 2.2.1: ok; one numeric minimum lookup used by parsing and error output.
- 2.2.2: ok individually; its shared-file changes need ordering against 2.3.1.
- 2.2.3: ok individually; AC7 and its generated test path are now valid, but its shared-file changes need ordering against 2.3.1.
- 2.3.1: change required; its dependency does not order its shared-file edits against 2.2.2 and 2.2.3.
- 2.3.2: ok; entry-point behavior and README update are covered by binary and documentation criteria.

#### Requested changes
1. In `docs/plan/phase-02-argument-parsing/2.3.1-print-arg-error.md:4`, add a dependency on 2.2.3 (which already depends on 2.2.2). This orders every subtask that changes `coders/main.h` and `coders/Makefile`; regenerate the plan index and `feature_list.json` with `make plan`.
2. In `docs/plan/phase-02-argument-parsing/2.1.2-parse-number.md:36`, change the minimum-check reference from 2.2.2 to 2.2.3, where `parse_config` actually applies `arg_minimum`.

## Round 3: phase gate, PHASE_APPROVED (2026-10-27)

See `progress/phase_review_02.md` (current verdict, unchanged since).

## Round 4: phase gate, CHANGES_REQUESTED (2026-10-27 16:15)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- **Coverage:** Tasks 2.1–2.4 cover the phase goal: numeric parsing, shared helpers, configuration validation, and CLI diagnostics. Phase 03 onward remains unbroken.
- **Scope:** Most subtasks are focused and declare one to three files. Subtask 2.2.4 declares four, contrary to the reviewer's mandatory micro-task limit. Subtask 2.2.3 would also leave a public function in a file with the wrong name at its close.
- **Dependencies:** The graph is acyclic and orders the shared header and Makefile edits. Core parsing precedes CLI integration; repeated file claims have explicit dependencies.
- **Acceptance:** Each subtask names one `tests/acceptance/test_<id>.c` path and has behavioral coverage. Several criteria instead require inspection of private implementation choices. The supplied `regate_check.sh phase 02` exited 0; its log attributes the underlying `make check` failures to this phase's changed seal. No tests were run in this sandbox.

#### Per subtask
- 2.1.1: ok; completed trim primitive with boundary and input immutability criteria.
- 2.1.2: ok; completed digits-only parser with overflow and failure-output criteria.
- 2.2.1: ok; focused move of usage output with preserved binary behavior.
- 2.2.2: ok; focused public-header layout change with API compatibility criteria.
- 2.2.3: `coders/src/parsing/trim_span.c` would define `trim_slice` until 2.2.5, violating the current one-public-function-per-file naming rule at the close of 2.2.3 (`2.2.3-rename-span-to-slice.md:7,34`; `docs/conventions.md`, Layout).
- 2.2.4: four declared files (`2.2.4-is-space.md:5-9`) exceed the mandatory one-to-three-file limit; AC4 also requires finding a private `static` symbol (`:14`). The exception written in the spec does not change the reviewer contract.
- 2.2.5: AC3 requires a particular private implementation (`strlen`, no helper or length loop) rather than an observable result (`2.2.5-move-trim-slice.md:12`).
- 2.2.6: ok; one predicate with boundary cases and its public prototype.
- 2.2.7: AC2 requires source inspection for `is_digit` and a forbidden expression rather than public behavior (`2.2.7-use-is-digit.md:9`).
- 2.3.1: ok; one minimum lookup with every position covered.
- 2.3.2: ok; one exact scheduler parser with invalid forms covered.
- 2.3.3: ok; one configuration parser with field masks and aggregate errors covered.
- 2.4.1: ok; one diagnostic function with exact stderr lines and boundary positions covered.
- 2.4.2: ok; entry-point integration, error ordering, valid inputs, and README update covered.

#### Requested changes
1. Split or reorder 2.2.4 so each subtask declares at most three production files and still builds cleanly at every close. Update dependencies and acceptance paths as needed (`2.2.4-is-space.md:5-9`).
2. Revise 2.2.3 so its closed state satisfies the file/function naming convention, or change the approved convention through the appropriate plan gate before relying on a temporary exception (`2.2.3-rename-span-to-slice.md:34`).
3. Move private implementation requirements in 2.2.4 AC4, 2.2.5 AC3, and 2.2.7 AC2 to the descriptions or code-review checks. Keep acceptance criteria testable through the public API or observable build behavior (`2.2.4-is-space.md:14`, `2.2.5-move-trim-slice.md:12`, `2.2.7-use-is-digit.md:9`).

## Round 5: phase gate, CHANGES_REQUESTED (2026-10-27 16:28)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 2.1–2.4 cover the phase goal, from numeric primitives through configuration validation and CLI errors. Later phases remain unbroken.
- Scope and dependencies: each pending subtask declares one to three files; the dependency graph is acyclic, orders repeated file claims, and keeps core parsing ahead of CLI integration.
- Acceptance: each subtask names one acceptance test path. The migration still requires duplicated trimming logic, and one criterion inspects implementation artifacts rather than observable behavior. The supplied re-gate check exited 0; no tests were run in this sandbox.

#### Per subtask
- 2.1.1: ok; delivered trim behavior and oracle are specified.
- 2.1.2: ok; delivered numeric parser and oracle are specified.
- 2.2.1: ok; moves usage output while preserving behavior.
- 2.2.2: ok; defines the public header layout without changing the API.
- 2.2.3: `2.2.3-trim-slice.md:38-46` explicitly duplicates the existing `trim_span` algorithm until 2.2.5; this conflicts with the reviewer's DRY rule and `docs/conventions.md` (Clean code).
- 2.2.4: ok; one caller switches to the new public helper.
- 2.2.5: AC1 tests the public API, but AC2 (`2.2.5-remove-trim-span.md:11`) inspects source-file absence and a Makefile entry. Those are implementation details; file removal belongs in code review.
- 2.2.6: ok; one whitespace predicate with boundary cases.
- 2.2.7: ok; preserves observable trim behavior after reuse.
- 2.2.8: ok; one digit predicate with boundary cases.
- 2.2.9: ok; preserves observable number parsing after reuse.
- 2.3.1: ok; minimums cover all argument positions.
- 2.3.2: ok; exact scheduler values and invalid forms are covered.
- 2.3.3: ok; config fields, per-position errors, and combined mask are covered.
- 2.4.1: ok; error text and position boundaries are covered.
- 2.4.2: ok; binary behavior covers validation, error order, and usage.

#### Requested changes
1. Revise the span-to-slice migration so each closed subtask reuses one trimming implementation; do not require `trim_span` and `trim_slice` to duplicate the algorithm (`2.2.3-trim-slice.md:38-46`). Keep the one-to-three-file limit and buildable state at every step.
2. Move 2.2.5 AC2's source-file and Makefile checks to its code-review instructions, and keep acceptance focused on the public API and behavior (`2.2.5-remove-trim-span.md:11`).

## Round 6: phase gate, CHANGES_REQUESTED (2026-10-27 16:34)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 2.1–2.4 deliver the phase goal, and their subtasks cover the stated tasks. Later phases remain unbroken.
- Scope and dependencies: all pending subtasks declare one to three production files, and the `depends` graph is acyclic and orders shared-file edits. The revised trim migration avoids duplicate trimming logic, but 2.2.3 reverses the required `parsing/` → `utils/` dependency until 2.2.5.
- Acceptance: every subtask names one acceptance test path. 2.2.1 AC2 checks source placement rather than observable behavior. The supplied re-gate check exited 0; no tests were run in this sandbox.

#### Per subtask
- 2.1.1: ok; delivered trim behavior and oracle are specified.
- 2.1.2: ok; delivered numeric parsing and oracle are specified.
- 2.2.1: AC2 inspects which source file defines `print_usage` and what functions `main.c` defines; move this to code review (`docs/plan/phase-02-argument-parsing/2.2.1-move-print-usage.md:11`).
- 2.2.2: ok; header layout and unchanged API are specified.
- 2.2.3: `trim_slice` in `utils/` delegates to `trim_span` in `parsing/`, contradicting the explicit module rule that `utils/` depends on nothing (`docs/plan/phase-02-argument-parsing/2.2.3-trim-slice.md:43-46`; `docs/architecture.md:25-26`).
- 2.2.4: ok; parser migration preserves observable behavior.
- 2.2.5: ok; one trimming implementation with old and new public behavior checked.
- 2.2.6: ok; removal of the old API and compatibility step are specified.
- 2.2.7: ok; whitespace predicate and boundary cases are specified.
- 2.2.8: ok; trim behavior is preserved after predicate reuse.
- 2.2.9: ok; digit predicate and boundary cases are specified.
- 2.2.10: ok; number parsing is preserved after predicate reuse.
- 2.3.1: ok; numeric minimums and invalid positions are specified.
- 2.3.2: ok; exact scheduler values and invalid forms are specified.
- 2.3.3: ok; config values, each error position, combined masks and input preservation are specified.
- 2.4.1: ok; error text and position boundaries are specified.
- 2.4.2: ok; binary behavior covers validation, error order, usage and documentation.

#### Requested changes
1. Rework 2.2.3–2.2.5 so `utils/` never calls `parsing/` at any closed subtask. Preserve a single trimming implementation, the one-to-three-file limit, and a buildable state at each step.
2. Remove 2.2.1 AC2 from acceptance and put its source-placement checks in the spec's code-review instructions. Keep acceptance criteria about the public API and observable behavior.

## Round 7: phase gate, CHANGES_REQUESTED (2026-10-27 16:39)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 2.1–2.4 cover the phase goal; later phases remain unbroken. The revised trim migration keeps `utils/` independent of `parsing/` and has a single trimming implementation at each step.
- Scope and dependencies: each pending subtask declares one to three production files; shared-file edits are ordered and the dependency graph is acyclic. The `trim_span` removal has no complete compatibility path for earlier acceptance oracles.
- Acceptance: each subtask names one acceptance test path, but 2.2.7 contradicts earlier criteria that require the old API. The README criterion in 2.4.2 conflicts with the required close order. The supplied re-gate check exited 0; no tests were run in this sandbox.

#### Per subtask
- 2.1.1: ok; delivered trim behavior and oracle exist.
- 2.1.2: ok; delivered numeric parser and oracle exist.
- 2.2.1: ok; the revised AC2 exercises the public `print_usage` API without `main.c`.
- 2.2.2: its oracle may call `trim_span`; include it in the 2.2.7 compatibility plan.
- 2.2.3: AC2 requires the `trim_span` prototype; include this obsolete structural check in the 2.2.7 compatibility plan.
- 2.2.4: AC5 requires the old API to compile, conflicting with 2.2.7 AC1.
- 2.2.5: ok; parser migration preserves observable behavior.
- 2.2.6: AC2 requires `trim_span`, conflicting with 2.2.7 AC1.
- 2.2.7: AC1 removes the old API while AC2 says the 2.2.4 and 2.2.6 oracles still pass; its gate step only updates the 2.1.1 oracle (`docs/plan/phase-02-argument-parsing/2.2.7-remove-trim-span.md:10-11,33-37`).
- 2.2.8: ok; predicate behavior and public declaration are specified.
- 2.2.9: ok; trim behavior is preserved after predicate reuse.
- 2.2.10: ok; predicate behavior and public declaration are specified.
- 2.2.11: ok; number parsing is preserved after predicate reuse.
- 2.3.1: ok; numeric minimums and invalid positions are specified.
- 2.3.2: ok; exact scheduler values and invalid forms are specified.
- 2.3.3: ok; config values, individual and combined error masks, and input preservation are specified.
- 2.4.1: ok; error text and position boundaries are specified.
- 2.4.2: AC7 requires a README update in the acceptance test before the code gate, but `AGENTS.md:62-65` assigns README edits to Codex at close (`docs/plan/phase-02-argument-parsing/2.4.2-validate-arguments.md:7,15,34-35`).

#### Requested changes
1. Resolve the 2.2.7 API removal against **every** prior acceptance oracle that requires `t_span` or `trim_span`, including 2.2.2, 2.2.3, 2.2.4 and 2.2.6 as well as 2.1.1. Specify how each sealed oracle and obsolete structural criterion will remain valid or be migrated by Codex at the plan gate; do not claim an oracle still passes while it requires a removed API.
2. Align 2.4.2 AC7 with the project workflow: Codex writes `README.md` at close, after the code gate requires green acceptance tests. Make README content a close obligation rather than a pre-close acceptance test, while retaining it in `files:` as a living file.

## Round 8: phase gate, CHANGES_REQUESTED (2026-10-27 16:42)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage and layers: tasks 2.1–2.4 cover the phase goal; the later phases retain only titles and goals. The utils → parsing → CLI dependency direction is sound.
- Scope and dependencies: each pending subtask declares one to three production files, shared-file edits are ordered, and the dependency graph is acyclic. The span-to-slice transition still breaks earlier acceptance checks at intermediate steps.
- Acceptance: each spec names one `tests/acceptance/test_<id>.c` path, but the planned deletion of checks in 2.2.7 would leave criteria without tests. The README obligation in 2.4.2 is correctly assigned to close. The supplied `scripts/regate_check.sh phase 02` exited 0; no tests were run in this sandbox.

#### Per subtask
- 2.1.1: delivered behavior and oracle exist; its AC6 oracle checks the old Makefile source path and needs migration when that path changes.
- 2.1.2: delivered numeric parser and oracle exist.
- 2.2.1: ok; the usage move has a callable public API check.
- 2.2.2: AC1 claims the 2.1.1 oracle still passes, but moving the source changes the path checked in `tests/acceptance/test_2.1.1.c:92`.
- 2.2.3: its AC2 names exactly the current three typedefs and the old `trim_span` declaration; later header changes need explicit oracle compatibility.
- 2.2.4: adding `t_slice` changes the header checked by 2.2.3; its own AC5 requires the old API to compile, conflicting with 2.2.7 unless that criterion receives a compatible successor check.
- 2.2.5: ok; parser migration preserves behavior.
- 2.2.6: its AC2 requires `trim_span`; removing that check in 2.2.7 leaves AC2 without a test.
- 2.2.7: the gate step covers old API names, but omits obsolete source-path checks and drops entire prior checks instead of preserving criterion-to-test traceability.
- 2.2.8: ok; whitespace predicate is specified and ordered after removal of the old helper.
- 2.2.9: ok; trim behavior is preserved after predicate reuse.
- 2.2.10: ok; digit predicate and its boundaries are specified.
- 2.2.11: ok; numeric behavior is preserved after predicate reuse.
- 2.3.1: ok; per-position minimums are specified.
- 2.3.2: ok; exact scheduler values and invalid forms are specified.
- 2.3.3: ok; config values, error masks, and input preservation are specified.
- 2.4.1: ok; error text and position boundaries are specified.
- 2.4.2: ok; binary behavior is observable and README work is a close obligation.

#### Requested changes
1. In 2.2.2, schedule Codex migration and re-sealing of the existing 2.1.1 oracle before implementation. Its AC6 test asserts `src/parsing/trim_span.c` appears in `coders/Makefile` (`tests/acceptance/test_2.1.1.c:92`), which will be false after the move (`docs/plan/phase-02-argument-parsing/2.2.2-move-trim-span.md:6-10`). The 2.2.7 step must also account for the later removal of `src/utils/trim_span.c`.
2. Give each earlier oracle a viable check at every transition. In particular, address 2.2.3 AC2 when 2.2.4 adds a fourth typedef, then when 2.2.7 removes `t_span`/`trim_span`; specify when Codex will migrate and re-seal its oracle (`docs/plan/phase-02-argument-parsing/2.2.3-header-sections.md:9`, `docs/plan/phase-02-argument-parsing/2.2.4-trim-slice.md:7`, `docs/plan/phase-02-argument-parsing/2.2.7-remove-trim-span.md:40-43`).
3. Rework the 2.2.7 compatibility plan so it preserves one meaningful test per acceptance criterion. It currently says to drop 2.2.4 AC5 and all of 2.2.6 AC2, leaving orphan criteria, and says other 2.1.1 checks remain identical despite its Makefile-path assertion (`docs/plan/phase-02-argument-parsing/2.2.7-remove-trim-span.md:38-43`). State the superseded criteria and replacement checks explicitly, with migration and re-seal steps.

## Round 9: phase gate, CHANGES_REQUESTED (2026-10-27 16:47)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 2.1–2.4 deliver the phase goal; later phases retain only titles and goals. The two completed subtasks have delivered code and acceptance tests.
- Scope and dependencies: each pending subtask has one focused unit and declares one to three files. Shared-file edits are ordered, the graph is acyclic, and dependencies run from utils through parsing to CLI.
- Acceptance: each subtask names one acceptance-test path. The revised 2.2.2 and 2.2.7 gate steps preserve and re-seal the six 2.1.1 checks across the source move and API removal. The 2.2.3 oracle still needs a compatibility plan. The supplied `scripts/regate_check.sh phase 02` exited 0; no tests were run in this sandbox.

#### Per subtask
- 2.1.1: delivered code and oracle exist; its AC6 migration is now specified in 2.2.2 and 2.2.7.
- 2.1.2: delivered code and oracle exist.
- 2.2.1: ok.
- 2.2.2: ok; its plan gate migrates and re-seals the 2.1.1 Makefile-path check before implementation.
- 2.2.3: AC3 requires the same types and prototypes as before, including `t_span` and `trim_span`, but no later gate step migrates this check when those names are removed.
- 2.2.4: ok in isolation; adding `t_slice` need not remove existing API.
- 2.2.5: ok.
- 2.2.6: ok; its criteria now use stable APIs.
- 2.2.7: its claim that only the 2.1.1 oracle references the old API overlooks 2.2.3 AC3. The scheduled search for old names does not specify a successor check for that criterion or its re-seal.
- 2.2.8: ok.
- 2.2.9: ok.
- 2.2.10: ok.
- 2.2.11: ok.
- 2.3.1: ok.
- 2.3.2: ok.
- 2.3.3: ok.
- 2.4.1: ok.
- 2.4.2: ok; README work is assigned to close.

#### Requested changes
1. Reconcile 2.2.3 AC3 with the planned removal of `t_span` and `trim_span` in 2.2.7. The criterion says the header keeps the same types and prototypes (`docs/plan/phase-02-argument-parsing/2.2.3-header-sections.md:10`), while 2.2.7 removes those names (`docs/plan/phase-02-argument-parsing/2.2.7-remove-trim-span.md:19-22`). State a meaningful successor check for 2.2.3 AC3 and schedule Codex migration and re-sealing of `tests/acceptance/test_2.2.3.c` before 2.2.7 implementation, or make AC3 explicitly about preserving the stable API so its oracle can remain valid. Update the assertion that only 2.1.1 needs oracle migration (`docs/plan/phase-02-argument-parsing/2.2.7-remove-trim-span.md:33-47`).

## Round 10: phase gate, PHASE_APPROVED (2026-10-27 16:48)

### Phase review — phase 02

**Verdict:** PHASE_APPROVED

#### Analysis
- Coverage: tasks 2.1–2.4 cover the phase goal, from numeric primitives and project conventions through config parsing and entry point validation. Later phases retain only titles and goals.
- Scope: each pending subtask has one focused change and declares one to three files. Shared-file edits are ordered by explicit dependencies; the dependency graph is acyclic and follows utils → parsing → CLI/main.
- Acceptance: every subtask has observable criteria and one `tests/acceptance/test_<N.M.K>.c` path. The revised 2.2.3 AC3 checks stable declarations, so it remains valid after the span-to-slice rename. The 2.2.2 and 2.2.7 plan-gate steps specify migration and re-sealing of the older 2.1.1 oracle before the corresponding implementation.
- Baseline: 2.1.1 and 2.1.2 are delivered work with existing code and acceptance tests. The supplied `scripts/regate_check.sh phase 02` result exited 0; no tests were run in this sandbox.

#### Per subtask
- 2.1.1: delivered `trim_span` implementation, header/Makefile entries, and six acceptance checks exist; later API migration is specified.
- 2.1.2: delivered `parse_number` implementation, header/Makefile entries, and six acceptance checks exist.
- 2.2.1: ok; moves one CLI function without changing its output.
- 2.2.2: ok; moves one helper and migrates the older Makefile-path check at the plan gate.
- 2.2.3: ok; section layout is testable, and AC3 covers only stable declarations.
- 2.2.4: ok; adds the new slice API while preserving the old one temporarily.
- 2.2.5: ok; switches the parser to the new API.
- 2.2.6: ok; transfers the single trim implementation while retaining old behavior.
- 2.2.7: ok; removes the old API after migrating and re-sealing its oracle.
- 2.2.8: ok; defines one generic whitespace predicate.
- 2.2.9: ok; reuses that predicate in trimming.
- 2.2.10: ok; defines one generic digit predicate.
- 2.2.11: ok; reuses that predicate in number parsing.
- 2.3.1: ok; centralizes numeric field minimums.
- 2.3.2: ok; accepts only the two exact scheduler names.
- 2.3.3: ok; composes the parsers and reports all invalid positions.
- 2.4.1: ok; maps each invalid position to its required error line.
- 2.4.2: ok; validates before simulation work and assigns the README update to close.

## Round 11: phase gate, CHANGES_REQUESTED (2026-10-27 17:56)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 2.1–2.4 cover numeric parsing, the convention refactor, config validation, and CLI errors. Later phases remain unbroken.
- Scope and dependencies: the subtasks are focused, declare one to three files, and order shared-file edits without a cycle. The behavior-preserving 2.2.5, 2.2.6, 2.2.9, and 2.2.11 specs now state the structural-refactor exception and give a source-inspection check for the code gate.
- Acceptance: each subtask names one acceptance-test path. The 2.2.7 removal step overlooks a sealed oracle that still requires the old source, so its proposed gate migration is incomplete.
- Supplied pre-session re-gate exited 0; no tests were run in this sandbox.

#### Per subtask
- 2.1.1: delivered implementation and acceptance oracle exist; its later migration is specified in 2.2.7.
- 2.1.2: delivered implementation and acceptance oracle exist.
- 2.2.1: ok; moves `print_usage` to CLI.
- 2.2.2: its sealed AC2 still asserts `src/utils/trim_span.c` is in `coders/Makefile`; 2.2.7 must name and migrate this oracle before removing that source.
- 2.2.3: ok; its header check accepts either trim prototype.
- 2.2.4: ok; adds the slice API.
- 2.2.5: ok; structural refactor with regression criteria and a code review check.
- 2.2.6: ok; structural refactor with regression criteria and a code review check.
- 2.2.7: incomplete older-oracle migration; see requested change.
- 2.2.8: ok; adds one whitespace predicate.
- 2.2.9: ok; structural refactor with regression criteria and a code review check.
- 2.2.10: ok; adds one digit predicate.
- 2.2.11: ok; structural refactor with regression criteria and a code review check.
- 2.3.1: ok; centralizes numeric minimums.
- 2.3.2: ok; parses the two exact modes.
- 2.3.3: ok; composes validation and returns all invalid positions.
- 2.4.1: ok; maps positions to specific errors.
- 2.4.2: ok; validates in `main` and specifies the README close update.

#### Requested changes
1. In `docs/plan/phase-02-argument-parsing/2.2.7-remove-trim-span.md`, correct the claim that only `test_2.1.1.c` references the old API/source. Explicitly migrate `tests/acceptance/test_2.2.2.c` AC2's Makefile assertion from `src/utils/trim_span.c` to the surviving `src/utils/trim_slice.c`, preserve its build/no-relink/Norm checks, and re-seal that oracle at the 2.2.7 plan gate. Its current assertion is at `tests/acceptance/test_2.2.2.c:57`.

## Round 12: phase gate, CHANGES_REQUESTED (2026-10-27 17:58)

### Phase review — phase 02

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 2.1–2.4 cover numeric parsing, the convention refactor, config validation, and CLI errors. Later phases remain unbroken.
- Scope and dependencies: the subtasks are focused, declare one to three files, and order shared-file edits without a cycle. The behavior-preserving 2.2.5, 2.2.6, 2.2.9, and 2.2.11 specs now state the structural-refactor exception and give a source-inspection check for the code gate.
- Acceptance: each subtask names one acceptance-test path. The 2.2.7 removal step now covers both sealed oracles affected by deleting the old source.
- Baseline: 2.1.1 and 2.1.2 have their declared implementation and acceptance files. The supplied harness log reports the build, Norm, and existing tests passing; no tests were run in this sandbox.
- Gate: the supplied re-gate exited 2. Its check failed because phase 02 is not approved for six done subtasks and the active subtask, and because four pending specs changed after the previous phase approval. The reviewer hard rule forbids approval with this red harness result.

#### Per subtask
- 2.1.1: delivered implementation and acceptance oracle exist; its later migration is specified in 2.2.7.
- 2.1.2: delivered implementation and acceptance oracle exist.
- 2.2.1: ok; moves `print_usage` to CLI.
- 2.2.2: ok; its later oracle migration is specified in 2.2.7.
- 2.2.3: ok; its header check accepts either trim prototype.
- 2.2.4: ok; adds the slice API.
- 2.2.5: ok; structural refactor with regression criteria and a code review check.
- 2.2.6: ok; structural refactor with regression criteria and a code review check.
- 2.2.7: ok; specifies migration and re-sealing of both affected older oracles.
- 2.2.8: ok; adds one whitespace predicate.
- 2.2.9: ok; structural refactor with regression criteria and a code review check.
- 2.2.10: ok; adds one digit predicate.
- 2.2.11: ok; structural refactor with regression criteria and a code review check.
- 2.3.1: ok; centralizes numeric minimums.
- 2.3.2: ok; parses the two exact modes.
- 2.3.3: ok; composes validation and returns all invalid positions.
- 2.4.1: ok; maps positions to specific errors.
- 2.4.2: ok; validates in `main` and specifies the README close update.

#### Requested changes
1. Resolve the red supplied harness gate before approval. `/tmp/harness-check-phase.QFA6Eb` reports the missing phase approval for 2.1.1–2.2.5 and invalidated seals for `2.2.6-move-trim-logic.md`, `2.2.7-remove-trim-span.md`, `2.2.9-use-is-space.md`, and `2.2.11-use-is-digit.md`. The plan content needs no further change from this review; the gate is self-referential because phase approval is needed to clear these failures.

## Round 13: phase gate, PHASE_APPROVED (2026-10-27 18:00)

### Phase review — phase 02

**Verdict:** PHASE_APPROVED

#### Analysis
- Coverage: tasks 2.1–2.4 deliver numeric primitives, the convention migration, config validation, and CLI errors. Every task has implementing subtasks. Later phases remain unbroken.
- Scope and dependencies: each subtask is focused, declares one to three files, and orders shared-file edits without a cycle. Core parsing precedes interface validation.
- Acceptance: each subtask names one acceptance-test path. The behavior-preserving 2.2.5, 2.2.6, 2.2.9, and 2.2.11 specs use the structural-refactor exception: regression oracles cover behavior and a `Code review check:` covers the structural change. The 2.2.7 removal step specifies migration of both sealed oracles affected by deleting the old source.
- Existing work: 2.1.1 and 2.1.2 have their declared implementation and acceptance files. The supplied `/tmp/harness-check-phase.fW2wZd` re-gate exited 0. Its build, Norm, and existing tests passed; its only excused `make check` failures are the phase approval and plan seals renewed by this gate. No tests were run in this sandbox.

#### Per subtask
- 2.1.1: delivered implementation and acceptance oracle exist; its later migration is specified in 2.2.7.
- 2.1.2: delivered implementation and acceptance oracle exist.
- 2.2.1: ok; moves `print_usage` to CLI.
- 2.2.2: ok; its later oracle migration is specified in 2.2.7.
- 2.2.3: ok; its header check accepts either trim prototype.
- 2.2.4: ok; adds the slice API.
- 2.2.5: ok; structural refactor with regression criteria and a code review check.
- 2.2.6: ok; structural refactor with regression criteria and a code review check.
- 2.2.7: ok; specifies migration and re-sealing of both affected older oracles.
- 2.2.8: ok; adds one whitespace predicate.
- 2.2.9: ok; structural refactor with regression criteria and a code review check.
- 2.2.10: ok; adds one digit predicate.
- 2.2.11: ok; structural refactor with regression criteria and a code review check.
- 2.3.1: ok; centralizes numeric minimums.
- 2.3.2: ok; parses the two exact modes.
- 2.3.3: ok; composes validation and returns all invalid positions.
- 2.4.1: ok; maps positions to specific errors.
- 2.4.2: ok; validates in `main` and specifies the README close update.
