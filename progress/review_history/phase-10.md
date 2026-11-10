# Review history: phase-10

## Round 1: phase gate, CHANGES_REQUESTED (2026-11-09 18:01)

### Phase review — phase 10

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: startup, subject checks and README are represented, but the claimed full-binary FIFO/EDF ordering evidence is absent. The verification subtasks otherwise follow the existing orchestration and interface layers.
- Scope: the new helper and its caller are separate atomic changes; the test-only specs use the documented exception. Dependencies are acyclic, but the README ownership rule is unresolved.
- Acceptance: each spec names one acceptance-test path. Several criteria need correction before they can serve as reliable gates.

#### Per subtask
- 10.1.1: ok; one helper, three declared files, observable stop and error cases.
- 10.1.2: AC1–AC2 depend on a heuristic 1 ms start offset to prevent burnout at `5 1000 ...`; the spec itself says the offset cannot guarantee the start order (10.1.1, lines 37–39). The README file is declared for Codex to edit at close.
- 10.2.1: ok as finite regression evidence for the stated sufficient bound; its empty `files:` uses the test-only exception.
- 10.2.2: the full-binary checks cover log and resource behaviour, but AC6's two-coder alternation does not establish FIFO or EDF request priority under contention. The spec acknowledges that request order cannot be inferred from logs (line 18).
- 10.2.3: ok; one narrow fixed-text output change across three declared CLI files, with byte-preserving criteria.
- 10.2.4: AC6 says the test always passes, so it is diagnostic output rather than an acceptance criterion. AC7 accepts any result except exit 42, including a missing or failed Valgrind invocation. The stated code-review check does call for separate memcheck and TSan results.
- 10.3.1: its implementer-owned README change conflicts with the AGENTS.md living-files rule assigning README updates to Codex at close; a spec cannot create an exception to that rule. The README content and usage criteria are otherwise observable.

#### Requested changes
1. Reconcile README authorship in 10.3.1 with AGENTS.md and the required green code gate. Change the governing workflow explicitly if implementer ownership is intended; also clarify why 10.1.2 declares the same file for a Codex close update.
2. In 10.1.2, replace or qualify AC1–AC2 so their mandatory pass condition does not require the non-guaranteed start order at a burnout time below the PLAN.md sufficient bound. Keep the empirical outcome as a diagnostic if useful.
3. Align the phase goal's FIFO/EDF ordering claim with evidence: add a controlled acceptance check of request ordering under contention, or state that the already sealed phase 06 API oracles provide that evidence and narrow the full-binary claim accordingly.
4. Move 10.2.4 AC6 out of `acceptance:` or make it a meaningful pass/fail condition. Strengthen AC7 to require that Valgrind actually ran and its target processes produced the expected exit statuses, while retaining the configured leak/error exit check.

## Round 2: phase gate, CHANGES_REQUESTED (2026-11-09 18:37)

### Phase review — phase 10

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: the tasks cover startup, subject verification, and README content. The revised phase goal correctly assigns exact FIFO/EDF request-order evidence to the sealed phase 06 API oracles. The claimed viability-boundary and interleaved-ticket cases are not yet established by their specified tests.
- Scope and dependencies: the production changes are narrow; the test-only exception permits empty `files:` or the root `Makefile`. Dependencies are acyclic. AGENTS.md now explicitly permits an implementer-owned README subtask, so 10.3.1 and the Codex close update in 10.1.2 have distinct owners.
- Acceptance: every subtask names its own test path. Several timing and symbol criteria still need correction to be reliable oracles. The supplied pre-session harness check exited 0; no tests were run in this review.

#### Per subtask
- 10.1.1: ok; one helper, three declared files, and observable stop/error cases.
- 10.1.2: AC1–AC2 remain mandatory when `S=0` despite relying on odd threads running within a 1 ms offset, which the spec itself says is not guaranteed. A loaded local machine can fail the test without a product defect (lines 9–10, 17). AC3–AC5 and file ownership are sound.
- 10.2.1: AC2–AC4 are mislabeled as boundary cases. The stated formula gives 1410 ms for N=5/C=200/R=200/D=0 and N=4/C=200/R=200/D=50, and 810 ms for N=2/C=200/R=200/D=0; the specs use 1610, 1610, and 1010 ms (lines 8–10, 16–19). AC1 calls the N=6 run “interleaved tickets,” but neither its assertions nor the 1 ms heuristic establishes ticket order (lines 7, 24–26).
- 10.2.2: ok for observable log, resource, cooldown, and lifecycle rules; the spec correctly leaves exact FIFO/EDF request order to sealed phase 06 tests.
- 10.2.3: ok; the three CLI edits are one narrowly defined fixed-text output correction, with byte-preserving checks.
- 10.2.4: AC3 repeats the non-guaranteed start-order assumption for mandatory local assertions and becomes diagnostic-only in CI (line 10). AC2 checks only externally visible data symbols, so a file-scope `static` data object could violate the subject's no-global rule without failing it (lines 9, 17). AC6 now requires actual Valgrind execution and target statuses when installed; the separate memcheck/TSan gate is stated.
- 10.3.1: ok; the README subtask exception now resolves authorship, and the content criteria and code-review check cover subject section 7.

#### Requested changes
1. Correct 10.2.1 AC2–AC4 to use the actual boundary values or label the current values as above-bound cases and add genuine boundary coverage. For the N=6 interleaved case, use controlled request admission or an observable oracle that establishes interleaved ticket order; otherwise remove that specific claim from the phase goal and spec.
2. Replace 10.1.2 AC1–AC2 and 10.2.4 AC3 as mandatory pass/fail tests with outcomes that do not depend on a 1 ms scheduling assumption. Keep the tight runs as diagnostics if useful, and retain deterministic checks that `run_coder` invokes and handles the offset.
3. Make 10.2.4 AC2 cover all file-scope global data, including internal-linkage objects, while distinguishing them from function-local constants.

## Round 3: phase gate, PHASE_APPROVED (2026-11-09 19:09)

### Phase review — phase 10

**Verdict:** PHASE_APPROVED

#### Analysis
- Coverage: startup staggering, full-binary subject checks, and the final README cover the phase goal. Exact FIFO/EDF request order remains covered by the sealed phase 06 API oracles; phase 10 does not claim that log output reveals ticket order.
- Scope and dependencies: each production subtask has at most three declared files; the empty `files:` in 10.2.1 and 10.2.2 and the dev-only `Makefile` in 10.2.4 use the stated test-only exception. The README subtask uses the stated authorship exception. Dependencies are acyclic and follow the implementation order. No later phase is broken down.
- Acceptance: every subtask has observable criteria and one dedicated `tests/acceptance/test_<id>.c` path. The viability values in 10.2.1 match `C + R + N * (C + D) + 10`; the N = 6 run establishes a full-binary boundary case without asserting unobservable ticket order. The startup offset and below-bound runs are described as empirical where scheduling matters. The supplied pre-session `make check` exited 0; no tests were run in this review.

#### Per subtask
- 10.1.1: ok; one startup-wait helper with stop, error, and no-side-effect checks.
- 10.1.2: ok; direct stopped-state and clock-error checks establish the call order and error path without relying on a 1 ms thread schedule; parallel-start runs are empirical.
- 10.2.1: ok; test-only full-binary checks at and above the stated EDF boundary, including N = 6.
- 10.2.2: ok; observable log, acquisition, cycle, exclusion, cooldown, fairness, burnout, and target checks.
- 10.2.3: ok; the three fixed-text CLI calls form one narrow allowed-function correction with byte-preserving checks.
- 10.2.4: ok; binary symbol and file-scope data checks, deterministic impossible and invalid runs, Valgrind checks, and code-gate memcheck/TSan evidence are specified. Function-local statics are distinguished from file-scope data.
- 10.3.1: ok; README-only deliverable with subject section 7 content, real-output, and AI-use checks.
