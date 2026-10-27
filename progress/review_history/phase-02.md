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
