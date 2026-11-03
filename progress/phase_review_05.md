# Phase review — phase 05

**Verdict:** PHASE_APPROVED

## Analysis
- Coverage: tasks 5.1–5.4 and all 11 specs cover ordered seat mapping, dongle and coder lifecycle, full simulation setup and reverse teardown, the single-coder case, and rollback after partial initialisation.
- Scope and dependencies: each subtask owns one public function and declares its source, `coders/main.h`, and `coders/Makefile`. Explicit dependencies sequence the shared files and the teardown helpers needed for rollback. The graph is acyclic and follows core-to-adapter prerequisites.
- Acceptance: each spec has observable public-API criteria and one distinct `tests/acceptance/test_<id>.c` path. Failure paths that cannot be forced portably are stated as code-review checks. No baseline is claimed, and later phases remain unbroken.
- Subject and architecture: per-dongle mutexes, conditions and heaps, per-coder mutexes, and simulation stop, log and ticket mutexes have explicit ownership and teardown. The one-coder case uses one dongle. The revised `init_simulation` name avoids the seven existing phase 03 oracle helpers named `init_sim`.
- Gate sequencing: the supplied outside-sandbox `make check` exited 2. Its remaining acceptance build failure is `test_5.4.2.c` calling the old `init_sim` name; that oracle must be updated and resealed at the 5.4.2 plan gate. Changed phase-plan seals are the reason for this phase re-review. Phase approval covers the breakdown; the 5.4.2 code gate and close still require a green check.

## Per subtask
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
- 5.4.2: ok — whole-simulation construction with reverse rollback; `init_simulation` is the public name.
