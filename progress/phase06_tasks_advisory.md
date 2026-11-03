# Phase 06 tasks — Codex advisory review (2026-11-03)

Advisory only (no gate, no seal). Input: progress/phase06_tasks_proposal.md.

# Advisory review — phase 06 task proposal

The two heap acquisition rule matches the sealed plan: lock dongles in index order, enqueue one identical request in both heaps, check both heads and cooldown, then pop and mark both unavailable in one critical section. The proposal needs these changes before specs are written:

1. **Define failure cleanup.** If the second push fails, or a queued waiter hits a clock or mutex error, its request can remain in one or both heaps and block other coders. The heap has no arbitrary removal operation. Specify rollback or a fatal stop path that wakes and ends every waiter; require release of both locks on every return. Also say how `take_dongles` handles a stop racing with its final readiness check.

2. **Make the lock protocol explicit.** The timed wait releases the first dongle mutex while the second is already unlocked; relocking the second afterward preserves index order. Checking stop while holding the first mutex, followed by a stop broadcast that locks it, prevents a lost stop wake up. Require that `take_dongles`, `release_dongles`, and `wake_dongles` never acquire `log_mutex` or a coder mutex while holding dongle mutexes. Existing logging uses `log_mutex → stop_mutex`; the proposed path uses `dongle mutexes → stop_mutex`. Keep ticket and coder locks outside the dongle critical section, and log only after acquisition unlocks both. Guard every read and write of `available` and `released_at` with the corresponding dongle mutex.

3. **Specify timing precisely.** `released_at` must use milliseconds elapsed from `sim->start_ms`, matching `last_compile_start` and the EDF key. `get_deadline` instead needs absolute wall clock time for `pthread_cond_timedwait`. The first-cond-only, 1 ms slice is a valid bounded wait, but wakes on the second cond are observed only at the next timeout; include that latency in timing claims and handle deadline construction failures.

4. **Correct the single-coder contract.** Phase 07 says the lone coder *takes* its one dongle, logs once, then waits for burnout. Logging without acquiring it would violate that goal. Put the one-dongle acquisition and release ownership in phase 07, or provide a phase 06 single-dongle operation; phase 09 wires the lifecycle. Phase 07 owns the two “has taken a dongle” lines after successful two-dongle acquisition. Phase 08 calls `wake_dongles` after either stop condition; phase 09 must also wake waiters on thread-start failure.

5. **Tighten scope and tests.** Eight named functions are plausible at one public function per file, but `take_dongles` will need small helpers to meet the Norm’s 25-line, five-function-per-file and four-parameter limits; declare any additional production files and dependencies in the specs. `wake_` is absent from the closed verb list, so the proposed conventions change must land first. Keep results to `0/-1` plus out-parameters or use a named enum for success/stopped/error. Arbiter ACs can use controlled threads and mutex/condition handshakes to arrange queued requests and releases, inspect both heaps, and assert FIFO/EDF order, cooldown, and stop exit. Use timed waits only as hang guards, not as evidence of ordering; no finite test proves the PLAN.md starvation argument.

**Recommendation: OK with changes — define rollback/fatal-stop cleanup, lock and time contracts, real N=1 acquisition, phase ownership, Norm decomposition, naming, and deterministic AC synchronization.**
