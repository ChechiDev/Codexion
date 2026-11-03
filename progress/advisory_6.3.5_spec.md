# 6.3.5 spec — Codex advisory review (2026-11-03)

Advisory only (no gate, no seal). Applied: is_stopped error = stop (6.3.4, 6.3.5), AC3 exactly one request, independent AC2/AC4 setups, pop in the fatal list.

**Advisory review — 6.3.5**

1. **Contract:** The single-mutex stop and timed-wait protocol, `0`/`taken` return contract, and simulation-clock cooldown match 6.3.4. The absolute deadline for `pthread_cond_timedwait` uses the correct separate time base. One inconsistency needs resolving: the existing `is_stopped` returns `1` on a mutex error, so the promised fatal `-1` path cannot detect that failure. This affects 6.3.4 too.

2. **Need:** Keep the phase 06 function. The sealed phase 07 goal requires the lone coder to take its dongle before waiting to burn out; phase 06’s task goal explicitly owns single-dongle acquisition. Phase 07 should own the one log line and the wait without compiling. A queue has no fairness purpose for one coder, but using the arbiter’s readiness rule keeps acquisition and cooldown consistent.

3. **Acceptance:** The ACs cover first take, cooldown under both policies, contention, stop, and invalid input. Tighten AC3 to assert **exactly one** queued request and one ticket while blocked; AC1’s empty heap cannot prove there was no double push. Give AC2 and AC4 fresh setups rather than referring to the state of another AC. Use a handshake to establish that the request is queued before checking that the worker remains blocked. AC2 already covers release and re-take under EDF.

4. **Norm:** Feasible in one `.c` file with one public function and a small number of static helpers, provided every function stays within the 25-line and four-parameter limits. No forbidden production function is required.

5. **Wording/dependencies:** The listed dependencies are sufficient; 6.3.2 is deliberately bypassed for the one-mutex wait. Add `pop` to the enumerated fatal failures, and reconcile the `is_stopped` error behavior with the fatal-path promise in both acquisition specs.

**Recommendation: OK with changes — clarify the stop-error contract, strengthen the queued-request AC, make AC setups independent, and name the pop failure path.**
