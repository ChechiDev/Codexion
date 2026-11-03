# Phase 06 — task breakdown proposal (pre-gate)

Status: proposal by the leader, **not yet approved by the user**. Specs are
written one at a time after approval; then the formal phase gate
(`review_with_codex.sh phase 06`) runs.

Phase goal (sealed, `docs/plan/phase-06-dongle-arbitration.md`): atomic
two-dongle acquisition (a coder takes both only when it is the head of both
dongle heaps and both are available with cooldown elapsed, mutexes locked
lower-index first), release with cooldown, `pthread_cond_timedwait` waits with
no busy-wait and wake-all on stop, with arbiter-level tests (head-of-both rule,
fifo/edf ordering, cooldown, stop wake-up) as evidence for the PLAN.md progress
argument.

Inputs already built: `t_request {key, ticket, coder}`, heap API (phase 04),
`get_request_key(mode, ticket, last_compile_start, time_to_burnout)`,
`t_dongle {mutex, cond, heap(cap 2), available = 1, released_at = -1}`,
`t_coder {id, first_dongle < second_dongle, mutex, last_compile_start, ...}`,
`t_sim {next_ticket, ticket_mutex, stop_mutex, stopped, dongles, ...}`,
`is_stopped`, `set_stopped`, `get_time_ms`. Names below checked: no clash with
any helper in `tests/` or `coders/`.

## Tasks

| Task | Layer | Subtasks |
|---|---|---|
| 6.1 Requests | adapters | 6.1.1 `take_ticket(t_sim *sim, long *out)` · 6.1.2 `build_request(t_coder *coder, t_request *out)` |
| 6.2 Readiness | orchestration / time | 6.2.1 `is_dongle_ready(const t_dongle *d, int coder, long now, int cooldown)` · 6.2.2 `get_deadline(long ms, struct timespec *out)` |
| 6.3 Acquisition and release | orchestration | 6.3.1 `wait_for_dongles(t_coder *coder)` · 6.3.2 `take_dongles(t_coder *coder)` · 6.3.3 `release_dongles(t_coder *coder)` · 6.3.4 `wake_dongles(t_sim *sim)` |

8 micro-subtasks, 1-3 production files each (`.c` + `main.h` + `coders/Makefile`),
new module `coders/src/dongle/` (orchestration) plus `src/time/` for the deadline.

## Contracts

- **6.1.1 `take_ticket`**: under `ticket_mutex`, writes `next_ticket` to `*out`
  and increments it: one simulation-wide, strictly increasing ticket per
  request. -1 on NULL, lock failure or `next_ticket == LONG_MAX` (no overflow),
  `*out` untouched.
- **6.1.2 `build_request`**: reads `last_compile_start` under the coder mutex,
  takes a ticket, `key = get_request_key(mode, ticket, last_compile_start,
  time_to_burnout)`, `coder = coder->id`. -1 if any step fails (key -1 checked
  before anything is pushed), `*out` untouched.
- **6.2.1 `is_dongle_ready`** (caller holds the dongle mutex, no locking inside):
  1 when the heap head is `coder`, `available == 1` and the cooldown has elapsed
  (`released_at == -1` or `now - released_at >= cooldown`); else 0.
- **6.2.2 `get_deadline`**: absolute `timespec` = now (`gettimeofday`) + `ms`,
  normalised nanoseconds, for `pthread_cond_timedwait`. -1 on bad input or clock
  failure.
- **6.3.1 `wait_for_dongles`** (caller holds both mutexes): unlocks the second,
  `pthread_cond_timedwait` on the **first** dongle's cond with a deadline of
  `min(remaining cooldown, 1 ms slice)`, then relocks the second (lower index
  first is preserved: it already holds the first). Timeout is a normal return.
  The slice bounds the latency of a wake-up sent on the other dongle's cond
  (no busy-wait: the thread is blocked in the cond between checks).
- **6.3.2 `take_dongles`**: builds the request, locks both mutexes lower index
  first, pushes the same request into both heaps, then loops: stopped →
  unlock, return 1 (holds nothing); ready on both (6.2.1) → pop itself from
  both, `available = 0` on both, unlock, return 0; else `wait_for_dongles`.
  -1 on any failure, holding nothing. Stop is checked while holding the first
  dongle's mutex, so a stop broadcast cannot be lost.
- **6.3.3 `release_dongles`**: locks both lower index first, `available = 1`,
  `released_at = now` on both, broadcasts both conds, unlocks.
- **6.3.4 `wake_dongles`**: for every dongle, lock its mutex, broadcast its
  cond, unlock (called after `set_stopped`; phase 08 wires it to the monitor).
  Locking before the broadcast is what makes the stop wake-up lossless.

## Decisions for the user

1. **N = 1 is out of the arbiter.** `take_dongles` needs two distinct dongles
   and returns -1 when `first_dongle == second_dongle`. Phase 07 (coder
   routine) handles the lone coder: it logs "has taken a dongle" once and
   waits for the burnout (subject: one dongle, the coder cannot compile).
2. **New verb `wake_`** for 6.3.4 (`wake_dongles`), added first to
   `docs/conventions.md` in a `style/` branch, as was done in PR #8. The
   alternatives (`release_waiters`, `set_stopped_and_wake`) fit the existing
   verbs worse.
3. **Stale requests after stop are left in the heaps.** Every thread exits on
   stop and `free_sim` frees the heaps, so a coder that stops while waiting
   does not remove its request (the heap has no remove-by-coder operation).
4. **Arbiter scenario tests** (head-of-both, fifo/edf order, cooldown, stop
   wake-up) are the ACs of 6.3.2-6.3.4, written by Codex as threaded
   acceptance tests on a `t_sim` built with `init_simulation`.

## Changes after the Codex advisory review (`progress/phase06_tasks_advisory.md`)

- **Revised task 6.3 (5 subtasks, 9 in the phase):** 6.3.1 `wake_dongles` ·
  6.3.2 `wait_for_dongles` · 6.3.3 `take_dongles` · 6.3.4 `release_dongles` ·
  6.3.5 `take_single_dongle`. `wake_dongles` moves first because the fatal
  path of `take_dongles` uses it.
- **Fatal-stop cleanup:** a failure after the request is queued (second push,
  clock, mutex, deadline) cannot be rolled back (the heap has no remove-by-coder),
  so `take_dongles` unlocks both, calls `set_stopped` then `wake_dongles` and
  returns -1: the run ends cleanly (PLAN.md: pthread failures shut down
  cleanly). Every return path holds no dongle mutex.
- **Return contract:** `int take_dongles(t_coder *coder, int *taken)`: 0 with
  `*taken = 1` (both held) or `*taken = 0` (stopped, holds nothing); -1 on
  error. Fits the 0/-1 + out-parameter convention. A stop set after the last
  readiness check does not undo the take: the caller (phase 07) sees the stop
  on its next log and releases.
- **Lock protocol (in every spec of 6.3):** dongle mutexes always lower index
  first; while holding a dongle mutex only `stop_mutex` may be taken (via
  `is_stopped`); never `log_mutex`, a coder mutex or `ticket_mutex` (the
  request is built before locking). Every read/write of `available` and
  `released_at` holds that dongle's mutex. Logging happens after unlocking
  (phase 07).
- **Time base:** `released_at` and `now` in `is_dongle_ready` are ms elapsed
  since `sim->start_ms` (same base as `last_compile_start` and the edf key);
  `get_deadline` alone uses absolute wall-clock time for `timedwait`. The 1 ms
  slice latency (a wake sent on the second dongle's cond is seen at the next
  timeout) is stated in the spec and counted in the PLAN.md `margin`.
- **N = 1:** phase 07's sealed goal says the lone coder *takes* its dongle, so
  phase 06 provides `take_single_dongle(t_coder *coder, int *taken)`: one
  mutex, one push, waits for head + available + cooldown (or stop), takes it.
  `take_dongles` keeps rejecting `first_dongle == second_dongle`. Phase 07
  logs "has taken a dongle" (twice, or once for N = 1); phase 08 calls
  `wake_dongles` after either stop condition; phase 09 also on thread-start
  failure.
- **Norm:** each spec declares its helper split; if `take_dongles` needs a
  second public helper file it is declared in its spec.
- **Tests:** arbiter ACs use controlled threads with mutex/cond handshakes and
  inspect both heaps; timed waits only as hang guards, never as evidence of
  order. No finite test is claimed to prove the PLAN.md progress argument.
- **Verb `wake_`:** added to `docs/conventions.md` in a `style/` branch before
  6.3.1 (as PR #8 did).
- **Order change during the spec breakdown (2026-11-03):** 6.3.3 is `release_dongles` and 6.3.4 `take_dongles` (swapped), so the arbiter scenario tests of 6.3.4 and 6.3.5 release through the real function.
