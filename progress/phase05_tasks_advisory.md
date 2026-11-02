# Phase 05 tasks — Codex advisory review (2026-11-02)

Advisory only (no gate, no seal). Input: progress/phase05_tasks_proposal.md.

**Advisory review — phase 05 task breakdown**

The responsibilities are largely in the right phases. Phase 05 should establish state and ownership; request creation and ticket allocation belong in phase 06, coder activity in 07, monitor decisions in 08, and thread creation and joining in 09. The proposal should explicitly leave thread IDs unjoined and untouched by phase 05 teardown.

Before writing specs, tighten these contracts:

- **Fields and initial values:** Define `available = 1`, empty heaps, `last_compile_start = 0`, `compile_count = 0`, `stopped = 0`, and the initial ticket. `released_at = 0` could impose a cooldown before a dongle’s *first* use; specify a representation that distinguishes “never released.” `thread` is needed later, but must not be treated as valid during phase 05.
- **Seat mapping:** State whether `seat` is zero based and how it maps to the one based `coder.id` and `t_request.coder`. Compute the wraparound without `seat + 1` overflowing at `INT_MAX`. For N=1, the duplicate indices describe one resource; later code must lock and enqueue it once.
- **Rollback:** Each `destroy_*` function should receive only a fully initialized object. Specify exact initialization and reverse destruction order, and pass the count of *successfully initialized* array elements to `free_*`. A failed `pthread_cond_init` must not lead to cond destruction; a failed heap allocation must not lead to destruction of an uninitialized mutex. Check array allocation multiplication for overflow. Define who owns each allocation and what remains usable after an `init_*` failure.
- **Granularity:** Twelve subtasks is reasonable. `init_sim` is the one scope risk: its spec needs a short, explicit rollback sequence for three mutexes and both arrays, with helpers if required by the Norm’s 25 line and five functions per file limits. Keep each subtask’s declared production files within the reviewer’s 1–3 file rule.
- **Naming:** The proposed verbs match `docs/conventions.md`. Prefer `first_dongle` and `second_dongle` over `first` and `second`, since these store ordered indices, not left and right ownership. State the `get_dongle_pair` result contract as `0/-1` with outputs unchanged on failure.

The proposal uses only allowed pthread and allocation functions and introduces no globals; the specs should preserve those limits.

**Recommendation: OK with changes — define initialization and ownership contracts, safe rollback counts and order, initial cooldown semantics, seat indexing, and clearer field names.**
