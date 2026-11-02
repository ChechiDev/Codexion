# Review history: phase-05

## Round 1: phase gate, CHANGES_REQUESTED (2026-11-02 17:25)

### Phase review — phase 05

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 5.1–5.4 and their 11 subtasks cover seat mapping, per-element and array lifecycle, and full simulation rollback. The single-coder case and reverse teardown are specified.
- Scope: each subtask owns one public function and declares its source file plus the living `coders/main.h` and `coders/Makefile`. Shared living files are explicitly sequenced by `depends` where required. No later phase is broken down.
- Dependencies: the graph is acyclic and follows the needed construction and teardown order. No baseline subtasks are declared.
- Acceptance: every spec names one `tests/acceptance/test_<id>.c` path and has useful public API checks. However, every spec also makes `nm` symbol inspection an acceptance criterion. Symbol-table inspection is an implementation detail, while these functions are already exercised through their public declarations. Build and Norm checks belong in the code gate, not in a behavioral acceptance oracle.

#### Per subtask
- 5.1.1: acceptance AC6 checks the binary symbol with `nm`.
- 5.2.1: acceptance AC7 checks the binary symbol with `nm`.
- 5.2.2: acceptance AC6 checks the binary symbol with `nm`.
- 5.2.3: acceptance AC6 checks the binary symbol with `nm`.
- 5.2.4: acceptance AC5 checks the binary symbol with `nm`.
- 5.3.1: acceptance AC6 checks the binary symbol with `nm`.
- 5.3.2: acceptance AC5 checks the binary symbol with `nm`.
- 5.3.3: acceptance AC6 checks the binary symbol with `nm`.
- 5.3.4: acceptance AC5 checks the binary symbol with `nm`.
- 5.4.1: acceptance AC7 checks the binary symbol with `nm`.
- 5.4.2: acceptance AC7 checks the binary symbol with `nm`.

#### Requested changes
1. Remove the `nm` symbol check from each listed acceptance criterion. Keep build and `norminette` verification in the code review checks or project gates, and keep the acceptance criteria focused on observable public API behavior. Preserve one acceptance test path per subtask and renumber criteria if needed.

## Round 2: phase gate, CHANGES_REQUESTED (2026-11-02 17:29)

### Phase review — phase 05

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 5.1–5.4 and their 11 subtasks cover seat mapping, per-element and array lifecycle, and full simulation rollback. The single-coder case and reverse teardown are specified.
- Scope: each subtask owns one public function and declares its source file plus the living `coders/main.h` and `coders/Makefile`. Shared living files are explicitly sequenced by `depends` where required. No later phase is broken down.
- Dependencies: the graph is acyclic and follows the needed construction and teardown order. No baseline subtasks are declared.
- Acceptance: every spec names one `tests/acceptance/test_<id>.c` path and has useful public API checks. However, every spec also makes `make -C coders` and `norminette coders/` an acceptance criterion. These are build and style gate checks, not observable public API behavior. They belong in code review checks or project gates, not a C behavioral acceptance oracle.

#### Per subtask
- 5.1.1: AC6 is a build/Norm criterion; other criteria are observable.
- 5.2.1: AC7 is a build/Norm criterion; other criteria are observable.
- 5.2.2: AC6 is a build/Norm criterion; other criteria are observable.
- 5.2.3: AC6 is a build/Norm criterion; other criteria are observable.
- 5.2.4: AC5 is a build/Norm criterion; other criteria are observable.
- 5.3.1: AC6 is a build/Norm criterion; other criteria are observable.
- 5.3.2: AC5 is a build/Norm criterion; other criteria are observable.
- 5.3.3: AC6 is a build/Norm criterion; other criteria are observable.
- 5.3.4: AC5 is a build/Norm criterion; other criteria are observable.
- 5.4.1: AC7 is a build/Norm criterion; other criteria are observable.
- 5.4.2: AC7 is a build/Norm criterion; other criteria are observable.

#### Requested changes
1. Remove the build/Norm AC from each listed spec. Keep build and `norminette` verification in the code review checks or project gates, and keep the acceptance criteria focused on observable public API behavior. Preserve one acceptance test path per subtask.

## Round 3: phase gate, CHANGES_REQUESTED (2026-11-02 17:33)

### Phase review — phase 05

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 5.1–5.4 and all 11 subtasks cover seat mapping, dongle and coder lifecycle, and simulation construction and teardown, including single-coder and partial-initialisation cases.
- Scope: each subtask specifies one function and three declared production files. The shared `coders/main.h` and `coders/Makefile` changes need an unambiguous dependency order.
- Dependencies: no cycle is present, but the graph permits 5.2.1 before 5.1.1 and 5.3.1 before 5.2.1. These pairs edit the same living files. In addition, 5.3.1 declares `t_coder` after `t_dongle`, which 5.2.1 introduces. See the `depends:` and `files:` lines in `docs/plan/phase-05-simulation-state/5.1.1-get-dongle-pair.md:4-8`, `5.2.1-init-dongle.md:4-8`, and `5.3.1-init-coder.md:4-8`.
- Acceptance: every spec has observable public API criteria and exactly one `tests/acceptance/test_<id>.c` path. Build and Norm checks have been moved to `Code review check:`. Later phases remain unbroken down; no baseline subtasks are declared.

#### Per subtask
- 5.1.1: ok.
- 5.2.1: add a dependency on 5.1.1 to sequence the shared header, Makefile, and `Init` section.
- 5.2.2: ok.
- 5.2.3: ok.
- 5.2.4: ok.
- 5.3.1: add a dependency on the dongle branch (for example 5.2.4) so `t_dongle` exists and the shared files have a single order.
- 5.3.2: ok.
- 5.3.3: ok.
- 5.3.4: ok.
- 5.4.1: ok.
- 5.4.2: ok.

#### Requested changes
1. Add `5.1.1` to the `depends:` list of 5.2.1.
2. Add `5.2.4` to the `depends:` list of 5.3.1. Keep its existing 5.1.1 dependency if desired. This sequences the shared files and ensures the `t_dongle` type exists before `t_coder` is declared after it.

## Round 4: phase gate, PHASE_APPROVED (2026-11-02 17:37)

### Phase review — phase 05

**Verdict:** PHASE_APPROVED

#### Analysis
- Coverage: tasks 5.1–5.4 and all 11 subtasks cover ordered seat mapping, dongle and coder lifecycle, whole-simulation construction and teardown, the one-coder case, and rollback after partial initialisation.
- Scope: each spec owns one function and declares three production files: its `.c` file plus the living `coders/main.h` and `coders/Makefile`. The shared files are sequenced by dependencies.
- Dependencies: the graph is acyclic and follows construction prerequisites. In particular, 5.2.1 depends on 5.1.1 and 5.3.1 depends on 5.2.4; teardown helpers precede array initialisers so rollback can reuse them.
- Acceptance: every spec has public API outcomes and one distinct `tests/acceptance/test_<id>.c` path. Failure paths that cannot be forced portably are explicit code-review checks. No baseline is claimed; later phases have only a title and goal.
- Subject and architecture: the state includes per-dongle mutex, condition variable and heap, per-coder mutex and back pointer, and simulation stop, log and ticket mutexes. The mapping handles one coder without a second dongle. Ownership and reverse cleanup are specified; no globals, threads, or unapproved functions are introduced in this phase.

#### Per subtask
- 5.1.1: ok — pure, overflow-safe ordered seat mapping.
- 5.2.1: ok — one dongle with reverse rollback; depends on 5.1.1.
- 5.2.2: ok — teardown of one fully initialised dongle.
- 5.2.3: ok — reverse teardown of a successful prefix.
- 5.2.4: ok — array construction with prefix rollback.
- 5.3.1: ok — one coder and its mutex; depends on the completed dongle branch.
- 5.3.2: ok — teardown of one fully initialised coder.
- 5.3.3: ok — reverse teardown of a successful prefix.
- 5.3.4: ok — array construction with prefix rollback.
- 5.4.1: ok — whole-simulation teardown, including partial arrays.
- 5.4.2: ok — whole-simulation construction with reverse rollback.
