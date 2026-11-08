# Review history: roadmap

> Rounds before 2026-10-27 were not recorded. The rounds below were
> reconstructed on 2026-10-27 from the session log (verdict texts verbatim;
> times not recorded).

## Round 1: roadmap gate, CHANGES_REQUESTED (2026-10-27)

### Roadmap review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- **Coverage:** The ten phases cover the build, parsing, concurrency, lifecycle, verification, and README goals. The revised Phase 02 goal introduces case-insensitive scheduler input outside the binding subject's allowed values.
- **Order and dependencies:** Build and parsing precede the shared runtime components; time/logging, heap, and state precede arbitration; coder and monitor precede lifecycle and full-binary verification. No phase goal depends on a later phase.
- **Size:** Each phase is a coherent increment. Phase 10 groups final system verification and documentation after the executable is integrated.
- **Architecture:** The overview keeps pure parsing and heap logic separate from platform adapters and thread orchestration, and names the timing, ownership, and synchronization risks.
- **Harness:** The supplied `make check` exit code is 2. Its build and tests passed; its two failures report that `PLAN.md` and the Phase 02 file changed after roadmap approval. This gate is reviewing those changes.

#### Requested changes
1. `docs/codexion-rules.md:139` requires the scheduler value to be exactly `fifo` or `edf`. `docs/plan/PLAN.md:143-150` and `docs/plan/phase-02-argument-parsing.md:2,10` instead accept `EdF` and values with surrounding whitespace. Restore exact scheduler matching in the roadmap and Phase 02 goal. Align the Phase 02 subtask specs and acceptance criteria before its phase gate, then rerun the roadmap gate.

## Round 2: roadmap gate, ROADMAP_APPROVED (2026-10-27)

### Roadmap review

**Verdict:** ROADMAP_APPROVED

#### Analysis
- **Coverage:** The ten phase goals cover the mandatory Codexion build, input validation, FIFO/EDF scheduling with cooldown, coder and monitor threads, lifecycle, verification, and README. They introduce no out-of-scope feature. The revised Phase 02 goal trims only numeric arguments; the scheduler remains exactly `fifo` or `edf`, as required by `docs/codexion-rules.md`.
- **Order and dependencies:** Build and parsing precede runtime components; time/logging, heap, and state precede arbitration; arbitration precedes coder and monitor threads; lifecycle precedes full-binary verification. No phase goal needs a later phase.
- **Size:** Each phase is a coherent increment. Phase 10 combines final system verification and documentation after the executable is integrated.
- **Architecture:** Pure parsing and heap logic remain separate from platform adapters and thread orchestration. The overview names the allowed-function constraint, ownership, synchronization, timing, and Norm risks.
- **Harness:** The supplied `make check` exited 2 because the changed roadmap lacks a current approval and seal. Its build and tests passed; this verdict reviews the changed roadmap for re-approval.

## Round 3: roadmap gate, CHANGES_REQUESTED (2026-10-27 15:50)

### Roadmap review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- **Coverage:** The ten phase goals cover the mandatory build, argument validation, scheduling and cooldown, coder and monitor threads, lifecycle, verification, and README. The revised `utils` and `cli` placement stays within the stated scope.
- **Order and dependencies:** Build and parsing precede runtime work; the heap and simulation state precede dongle arbitration; arbitration precedes the coder and monitor; full-binary verification follows lifecycle integration. No phase goal requires a later phase.
- **Size:** Each phase is a coherent increment. Phase 02's added conventions task prepares the parser and interface work without expanding the project's feature scope.
- **Architecture:** The overview keeps pure helpers, parsing, and heap logic separate from platform adapters and thread orchestration. It names the synchronization, timing, ownership, and Norm risks.
- **Gate:** The supplied `make check` exited 2. Compilation and tests passed, but integrity checks report that `docs/plan/PLAN.md` and the approved phase 02 plan changed after sealing. The reviewer role's hard rule forbids approval while the harness check is red.

#### Requested changes
1. Resolve the roadmap seal failure for `docs/plan/PLAN.md` and the phase 02 seal failures reported in `/tmp/harness-check-roadmap.Go0hpx` through the prescribed gate process, then provide a green `make check` result for roadmap re-review. Do not edit seal files manually.

## Round 4: roadmap gate, CHANGES_REQUESTED (2026-10-27 15:52)

### Roadmap review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- **Coverage:** The ten phase goals cover the mandatory subject, including parsing, FIFO/EDF arbitration and cooldown, coder and monitor threads, lifecycle, verification, and README. No phase goal is outside the stated scope.
- **Order and dependencies:** The build and parser precede runtime work; the heap and simulation state precede arbitration; arbitration precedes the coder and monitor; integrated simulation tests follow lifecycle. No phase depends on a later one.
- **Size:** Each phase has a coherent goal. Phases 03–08 provide components that can be verified before full integration in phase 09.
- **Architecture:** The roadmap places parsing and heap logic in core, platform calls in adapters, and thread coordination in orchestration. It identifies the synchronization, timing, ownership, and Norm risks.
- **Gate:** The supplied `make check` result is exit code 2. It reports that phases 01 and 02 are broken down without `ROADMAP_APPROVED`, and that `docs/plan/PLAN.md` changed after roadmap approval. Compilation and tests passed. The reviewer role forbids approval while this check is red.

#### Requested changes
1. Resolve the roadmap gate and plan-seal failures reported in `/tmp/harness-check-roadmap.ouxGx2` through the prescribed gate workflow, then provide a green external `make check` result for re-review. Do not alter seal files manually.

## Round 5: roadmap gate, ROADMAP_APPROVED (2026-10-27 16:13)

### Roadmap review

**Verdict:** ROADMAP_APPROVED

#### Analysis
- **Coverage:** The ten phase goals deliver the mandatory subject: build and validation, time and logs, a priority heap, simulation state, cooldown and FIFO/EDF dongle arbitration, coder and monitor threads, lifecycle, full-binary verification, and the required README. Nothing in the phase goals exceeds the stated scope.
- **Order and dependencies:** Foundations and core logic precede state and thread coordination; arbitration precedes the coder and monitor routines; lifecycle precedes integrated hardening. No phase goal requires work assigned to a later phase.
- **Size:** Each phase has a coherent, independently verifiable goal. The final phase validates the integrated program and finishes its documentation.
- **Architecture:** The overview keeps pure parsing, heap, and policy logic in core; time, logging, and resource lifecycle at the platform edge; and CLI in the interface. It names the concurrency protocol, ownership, timing precision, and Norm risks.
- **Gate:** The supplied external `./scripts/regate_check.sh roadmap` exited 0. Its check found only plan approval and seal failures excused for this re-review; compilation and tests passed. Phase 02's changed seal still requires its own phase gate.

## Round 6: roadmap gate, ROADMAP_APPROVED (2026-10-28 16:26)

### Roadmap review

**Verdict:** ROADMAP_APPROVED

#### Analysis
- **Coverage:** The ten phase goals deliver the mandatory subject: build and validation, time and logs, a priority heap, simulation state, cooldown and FIFO/EDF dongle arbitration, coder and monitor threads, lifecycle, full-binary verification, and the required README. Nothing in the phase goals exceeds the stated scope.
- **Order and dependencies:** Foundations and core logic precede state and thread coordination; arbitration precedes the coder and monitor routines; lifecycle precedes integrated hardening. No phase goal requires work assigned to a later phase.
- **Size:** Each phase has a coherent, independently verifiable goal. The final phase validates the integrated program and finishes its documentation.
- **Architecture:** The overview keeps pure parsing, heap, and policy logic in core; time, logging, and resource lifecycle at the platform edge; and CLI in the interface. The revised logger uses the subject-allowed `write` function for immediate, mutex-serialized output, with decimal formatting in a pure utility. It names the concurrency protocol, ownership, timing precision, and Norm risks.
- **Gate:** The supplied external `./.claude/scripts/regate_check.sh roadmap` exited 0. Its check found only the roadmap plan seal failure excused for this re-review; compilation and tests passed. Phase 03's new breakdown still requires its separate phase gate.

## Round 7: roadmap gate, CHANGES_REQUESTED (2026-11-08 16:52)

### Roadmap review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- **Coverage:** The ten phase goals cover the mandatory subject, input validation, FIFO/EDF arbitration with cooldown, coder and monitor threads, lifecycle, full-binary verification, and the required README. No phase goal falls outside the stated scope.
- **Order and size:** Build, parsing, time, queue, and state precede arbitration; arbitration precedes the thread routines; lifecycle precedes integrated hardening. Each phase has a coherent goal and no phase depends on a later one.
- **Architecture:** The added `lifecycle` module in `docs/plan/PLAN.md:44,60` is orchestration, matching the phase 09 goal and `docs/architecture.md`. The plan names the main concurrency and timing risks.
- **Gate:** The supplied outside-sandbox `make check` exited 2. Compilation and tests passed, but the integrity check failed because `docs/plan/PLAN.md` changed after its roadmap seal. The reviewer role's hard rule forbids approval while the supplied check is red.

#### Requested changes
1. Reinvoke the roadmap review through the project's re-gate wrapper described in `docs/conventions.md` (style branches section), with an outside-sandbox check that accepts only the roadmap seal failure under re-review. Supply that green result to this gate; the roadmap content needs no change.

## Round 8: roadmap gate, ROADMAP_APPROVED (2026-11-08 16:55)

### Roadmap review

**Verdict:** ROADMAP_APPROVED

#### Analysis
- **Coverage:** The ten phase goals cover the mandatory subject, input validation, FIFO/EDF arbitration with cooldown, coder and monitor threads, lifecycle, full-binary verification, and the required README. No phase goal falls outside the stated scope.
- **Order and size:** Build, parsing, time, queue, and state precede arbitration; arbitration precedes the thread routines; lifecycle precedes integrated hardening. Each phase has a coherent goal and no phase depends on a later one.
- **Architecture:** The added `lifecycle` module in `docs/plan/PLAN.md:44,60` is orchestration, matching the phase 09 goal and `docs/architecture.md`. The plan names the main concurrency and timing risks.
- **Gate:** The supplied outside-sandbox `./.claude/scripts/regate_check.sh roadmap` exited 0. Its log shows the build and tests passed; the wrapper accepted only the expected roadmap seal and dependent approval failures while this roadmap is under re-review.
