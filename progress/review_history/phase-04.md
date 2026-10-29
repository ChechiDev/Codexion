# Review history: phase-04

## Round 1: phase gate, PHASE_APPROVED (2026-10-29 18:11)

### Phase review — phase 04

**Verdict:** PHASE_APPROVED

#### Analysis
- Coverage: tasks 4.1–4.3 deliver request ordering and fifo/edf keys, fixed-capacity heap storage, and push/peek/pop operations required by the phase goal and the binding subject's hand-written priority queue requirement.
- Scope: each of the seven pending subtasks adds one public function, declares its source, `coders/main.h`, and `coders/Makefile`, and keeps tests in a single acceptance path. Shared header and Makefile edits have explicit sequential dependencies.
- Dependencies: the chain 4.1.1 → 4.1.2 → 4.2.1 → 4.2.2 → 4.3.1 → 4.3.2 → 4.3.3 has no cycle. The code remains in core; locking and two-dongle arbitration belong to later phases.
- Acceptance: every spec has observable public-API criteria for success, failure, edge cases, and state preservation. Each AC names its subtask's one acceptance file. Code review checks cover implementation constraints separately. Phases 05–10 remain unbroken. No baseline subtasks occur in this phase.
- The supplied outside-sandbox `make check` result was exit code 0. No tests were run in this review.

#### Per subtask
- 4.1.1: ok — strict lexicographic comparator, NULL handling, extremes, and read-only behavior are covered.
- 4.1.2: ok — fifo and edf keys, invalid inputs, and arithmetic boundary cases are covered.
- 4.2.1: ok — allocation ownership, capacity validation, and unchanged output on failure are specified; allocation-failure handling is reserved for code review.
- 4.2.2: ok — teardown, repeated free, NULL, and reuse are covered.
- 4.3.1: ok — insertion, heap invariant, capacity rejection, copy semantics, and larger heaps are covered.
- 4.3.2: ok — minimum lookup, unchanged heap, empty and invalid calls, and copy semantics are covered.
- 4.3.3: ok — ordered removal, heap invariant, interleaving, small capacities, and failure state are covered.
