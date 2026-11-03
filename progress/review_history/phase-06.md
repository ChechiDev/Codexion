# Review history: phase-06

## Round 1: phase gate, CHANGES_REQUESTED (2026-11-03 18:44)

### Phase review — phase 06

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage: tasks 6.1–6.3 cover request keys and tickets, readiness, timed waiting, atomic two-dongle acquisition, release, stop wake-up, and the one-coder path. Later phases retain only titles and goals. Each of the nine specs owns one public function and declares three production files and one distinct acceptance test path.
- Dependencies: the graph is acyclic, but independent subtasks edit both `coders/main.h` and `coders/Makefile` without an explicit sequencing dependency. In particular, 6.3.3 AC3 calls `is_dongle_ready` from 6.2.1 while 6.3.3 declares `depends: none`.
- Acceptance: 6.2.2 AC5 contradicts its own overflow contract on the 64-bit target. With `ms == LONG_MAX`, `ms / 1000` plus the current epoch fits in a signed 64-bit `tv_sec`, so the call need not fail. The 1 ms timeout in 6.3.2 also makes AC3's broadcast indistinguishable from a timeout by the stated observation.
- Subject and architecture: the proposed lock order, elapsed-time cooldown, wall-clock condition deadline, and stop broadcast are consistent with the binding subject and the approved plan. No baseline subtask is claimed. The supplied outside-sandbox `make check` exited 0; no tests were run in this review.

#### Per subtask
- 6.1.1: ok — one synchronized ticket operation with overflow and concurrency criteria.
- 6.1.2: ok — one request builder, dependent on ticket allocation.
- 6.2.1: dependency sequencing missing for shared living files; readiness rule and criteria are otherwise sound.
- 6.2.2: AC5 incorrectly requires `LONG_MAX` milliseconds to overflow; dependency sequencing is also missing.
- 6.3.1: dependency sequencing missing for shared living files; wake-all contract is otherwise sound.
- 6.3.2: AC3 cannot distinguish a signal from its 1 ms timeout as written; shared-file sequencing is incomplete.
- 6.3.3: AC3 requires 6.2.1 but `depends` is `none`; shared-file sequencing is incomplete.
- 6.3.4: ok — the stated prerequisites, atomic take rule, and arbiter scenarios cover its goal.
- 6.3.5: missing a dependency path from 6.3.4 for the shared header and Makefile; its one-dongle behavior is otherwise covered.

#### Requested changes
1. In `docs/plan/phase-06-dongle-arbitration/6.2.2-get-deadline.md:14`, remove the claim that `LONG_MAX` milliseconds necessarily overflows. State the intended behavior for a representable deadline, and keep overflow rejection conditional on an actual `time_t` limit. Update AC5 accordingly.
2. Add explicit `depends` paths sequencing all nine edits to `coders/main.h` and `coders/Makefile`, with core prerequisites before their callers and no cycles. At minimum, 6.3.3 must depend on 6.2.1 for its AC3, and 6.3.5 must follow 6.3.4 for the shared files. See the independent `depends: none` entries in 6.2.1, 6.2.2, 6.3.1, and 6.3.3.
3. Revise `docs/plan/phase-06-dongle-arbitration/6.3.2-wait-for-dongles.md:12` so AC3 has an observable result that distinguishes a condition signal from the specified 1 ms timeout, or move the choice of condition variable to its code-review check. Acquiring the first mutex and broadcasting does not establish which event caused the return.

## Round 2: phase gate, CHANGES_REQUESTED (2026-11-03 18:49)

### Phase review — phase 06

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage and scope: tasks 6.1–6.3 cover tickets, policy keys, readiness, timed waiting, atomic acquisition, release, stop wake-up, and the one-coder path. Each of the nine pending subtasks owns one public function, declares three production files, and names one distinct acceptance test path. Later phases have only titles and goals; there are no baseline subtasks.
- Dependencies: the graph is acyclic and now sequences every edit to `coders/main.h` and `coders/Makefile`. Callers follow their prerequisites, including `is_dongle_ready` before 6.3.3 and 6.3.4 before 6.3.5.
- Acceptance: the previously incorrect `LONG_MAX` deadline expectation was corrected, and 6.3.2 now checks the wait behavior without trying to infer a condition signal from a 1 ms timeout. However, 6.3.3 does not establish that cooldown begins when a dongle is actually released.
- Subject: `docs/codexion-rules.md:138,203` requires the full cooldown after release. The supplied outside-sandbox `make check` exited 0; no tests were run here.

#### Per subtask
- 6.1.1: ok — synchronized, bounded ticket allocation.
- 6.1.2: ok — one request builder with FIFO/EDF criteria.
- 6.2.1: ok — pure head, availability, and cooldown predicate.
- 6.2.2: ok — representable absolute deadlines and timeout use.
- 6.3.1: ok — broadcast, stop wake-up, and lost-wake-up scenarios.
- 6.3.2: ok — bounded condition wait and mutex restoration.
- 6.3.3: change needed — `released_at` is sampled before locking, so lock contention can consume part or all of the cooldown before release occurs.
- 6.3.4: ok subject to 6.3.3 — atomic two-dongle take and arbiter-level ordering scenarios.
- 6.3.5: ok subject to 6.3.3 — single-dongle take, wait, stop, and cooldown scenarios.

#### Requested changes
1. In `docs/plan/phase-06-dongle-arbitration/6.3.3-release-dongles.md:18`, require `get_elapsed_ms` after both required dongle mutexes have been acquired, immediately before making the dongles available. A clock failure must unlock the held mutexes and leave their fields unchanged. Update the description at lines 27–30 to define `released_at` as the actual release time. Add an observable contention case to the acceptance criteria: hold a dongle mutex while `release_dongles` runs in another thread, record the simulation time just before unlocking, and require `released_at` to be no earlier than that time. The current AC1 before/after window cannot detect a timestamp taken before the lock wait.

## Round 3: phase gate, PHASE_APPROVED (2026-11-03 18:52)

### Phase review — phase 06

**Verdict:** PHASE_APPROVED

#### Analysis
- Coverage: tasks 6.1–6.3 deliver ticket allocation, FIFO/EDF requests, readiness and deadlines, timed waiting, atomic acquisition, cooldown on release, stop wake-up, and the one-coder path. Their arbiter-level acceptance scenarios cover the phase goal.
- Scope: all nine pending subtasks specify one public function and three production files (`.c`, `coders/main.h`, `coders/Makefile`). Shared-file edits are ordered by explicit dependencies. There are no baseline subtasks.
- Dependencies: the graph is acyclic, callers follow their prerequisites, and the pure readiness rule remains in `policy/`; pthread and clock work remains in adapters or orchestration. Phases 07–10 retain only titles and goals.
- Acceptance: each subtask has observable criteria and one distinct `tests/acceptance/test_<id>.c` path. The 6.3.3 correction reads the simulation clock after both dongle mutexes are held (`docs/plan/phase-06-dongle-arbitration/6.3.3-release-dongles.md:18`), and AC6 measures release time under lock contention. This now establishes a full cooldown after actual release, as required by `docs/codexion-rules.md:203`. The supplied outside-sandbox `make check` exited 0; no tests were run in this review.

#### Per subtask
- 6.1.1: ok — bounded, synchronized ticket allocation.
- 6.1.2: ok — one policy request builder with observable FIFO/EDF keys.
- 6.2.1: ok — pure head, availability, and cooldown predicate.
- 6.2.2: ok — representable absolute deadlines and timed-wait use.
- 6.3.1: ok — wake-all and stop wake-up scenarios.
- 6.3.2: ok — bounded wait, second-mutex release, and mutex restoration.
- 6.3.3: ok — actual release timestamp, preserved queues, wake-up, and contention oracle.
- 6.3.4: ok — atomic two-dongle take, FIFO/EDF ordering, cooldown, stop, and progress scenarios.
- 6.3.5: ok — single-dongle take, cooldown, wait, and stop scenarios.
