# Phase 05 — task breakdown proposal (pre-gate, advisory review)

Status: proposal by the leader, **not yet approved by the user**. Specs per
subtask are written only after the user approves these tasks, one spec at a
time; then the formal phase gate (`review_with_codex.sh phase 05`) runs.

Phase goal (sealed, `docs/plan/phase-05-simulation-state.md`): build the
`t_sim` (coders, dongles with mutex/cond/heap, stop and log mutexes,
seat-to-dongle mapping with 1 dongle for 1 coder) and tear it down completely,
cleaning up partial initialisation on any malloc or pthread init failure
without leaks.

## Types (each enters `coders/main.h` with the first subtask that uses it)

- `t_dongle`: `pthread_mutex_t mutex`, `pthread_cond_t cond`, `t_heap heap`
  (capacity 2), `int available`, `long released_at`.
- `t_coder`: `int id` (1..N), `int first`, `int second` (its dongle indices,
  lower first), `pthread_mutex_t mutex`, `long last_compile_start`,
  `int compile_count`, `pthread_t thread`, `t_sim *sim` (back pointer: the
  thread routine receives a single argument).
- `t_sim` (exists since 3.2.1 with `start_ms`, `stopped`, `stop_mutex`,
  `log_mutex`) gains `t_config config`, `t_coder *coders`, `t_dongle *dongles`,
  `long next_ticket`, `pthread_mutex_t ticket_mutex` (the simulation-wide,
  synchronized, strictly increasing ticket fixed as an invariant in phase 04).
  `start_ms` stays 0 at init; phase 09 sets it when the threads start.

## Tasks

| Task | Layer | Subtasks |
|---|---|---|
| 5.1 Seat mapping | core | 5.1.1 `get_dongle_pair(seat, n, &first, &second)`: seat `i` uses dongles `i` and `(i+1)%N`, returned lower index first; with N=1 both are dongle 0 (single dongle, never locked or pushed twice) |
| 5.2 Dongles | adapters | 5.2.1 `init_dongle` (rolls back its own partial init) · 5.2.2 `destroy_dongle` · 5.2.3 `init_dongles` (array of N, rolls back the k-1 already built when the k-th fails) · 5.2.4 `free_dongles(dongles, count)` |
| 5.3 Coders | adapters | 5.3.1 `init_coder` · 5.3.2 `destroy_coder` · 5.3.3 `init_coders` (with rollback) · 5.3.4 `free_coders(coders, count)` |
| 5.4 Simulation | adapters | 5.4.1 `init_sim` (config, stop/log/ticket mutexes, `stopped = 0`, arrays, full rollback) · 5.4.2 `free_sim` |

12 micro-subtasks, 1-3 production files each (`.c` + `main.h` + `coders/Makefile`).

## Changes after the Codex advisory review (`progress/phase05_tasks_advisory.md`)

- **Field names:** `t_coder.first` / `second` → `first_dongle` / `second_dongle`
  (ordered indices, not left/right).
- **Initial values (fixed in the specs):** dongle `available = 1`, empty heap,
  `released_at = -1` = "never released" (no cooldown before the first take;
  phase 06 treats -1 as cooldown elapsed); coder `last_compile_start = 0`,
  `compile_count = 0`; sim `stopped = 0`, `next_ticket = 0`, `start_ms = 0`.
- **`thread`:** declared for phase 09 only; phase 05 never reads, joins or
  tears it down.
- **Seat indexing:** `seat` is 0-based (`0 <= seat < N`), `coder.id = seat + 1`
  (the number printed in the logs). Wraparound computed as
  `seat == N - 1 ? 0 : seat + 1` (no `seat + 1` overflow at `INT_MAX`).
  `get_dongle_pair` returns 0/-1 and leaves outputs untouched on failure
  (NULL outputs, `N < 1`, seat out of range). With N=1 both outputs are 0:
  one resource, locked and pushed once (phase 06).
- **Rollback contract:** `destroy_dongle` / `destroy_coder` only take a fully
  initialised object. `init_dongle` order: mutex → cond → heap; on failure it
  destroys only what succeeded, in reverse. `init_dongles` / `init_coders`
  check `N * sizeof` for overflow before `malloc`, and on failure of element k
  call `free_*` with count k (successfully initialised elements only), leaving
  the out-pointer untouched. `free_*(array, count)` destroys elements in
  reverse and frees the array.
- **`init_sim`:** its spec lists the exact sequence (stop, log, ticket mutexes
  → dongles → coders) and the reverse rollback; static helpers inside the file
  if the Norm (25 lines, 5 functions/file) requires.
- **Phase boundaries:** ticket allocation and request creation → phase 06;
  coder activity → 07; monitor → 08; thread creation/join → 09.
