# Phase 07 — task breakdown proposal (pre-gate)

Status: proposal by the leader, **not yet approved by the user**. Specs are
written one at a time after approval; then the formal phase gate
(`review_with_codex.sh phase 07`) runs.

Phase goal (sealed, `docs/plan/phase-07-coder-thread.md`): the coder routine
loops take dongles, compiling, release, debugging, refactoring with correct
logs, updates `last_compile_start` and `compile_count` under its own mutex, and
exits promptly on stop (single coder takes its only dongle and waits to burn out).

Inputs already built: `t_coder {id, first_dongle, second_dongle, mutex,
last_compile_start, compile_count, thread, sim}`, `print_state(sim, coder,
state)` (stop checked inside the log mutex), `sleep_ms(sim, ms)` (sliced,
returns -1 when stopped), `get_elapsed_ms`, `is_stopped`, `set_stopped`,
`wake_dongles`, `take_dongles(coder, &taken)`, `take_single_dongle(coder,
&taken)`, `release_dongles(coder)` (handles the single dongle). Names below
checked: no clash in `tests/` or `coders/`.

## Tasks (v2, after the Codex advisory `progress/phase07_tasks_advisory.md`)

| Task | Layer | Subtasks |
|---|---|---|
| 7.1 Coder state | orchestration | 7.1.1 `set_compile_start(t_coder *coder)` · 7.1.2 `set_compile_count(t_coder *coder)` |
| 7.2 Coder acquisition | orchestration | 7.2.1 `take_coder_dongles(t_coder *coder, int *taken)` |
| 7.3 Coder routine | orchestration | 7.3.1 `run_coder(void *arg)` (single-coder wait as a static helper) |

4 micro-subtasks, 1-3 files each (`.c` + `main.h` + `coders/Makefile`), new
module `coders/src/coder/`.

## Contracts

- **7.1.1 `set_compile_start`**: validates `coder` and `coder->sim`; under
  the coder mutex reads `get_elapsed_ms(sim->start_ms)` and sets
  `last_compile_start` (clock failure → unlock, -1, field untouched; changed
  after the 7.1.1 spec advisory to avoid a stale start); mutex released before
  any log or sleep. Called after the dongles are taken and before the
  `is compiling` line. 0 / -1.
- **7.1.2 `set_compile_count`**: under the coder mutex, `compile_count++` only
  after a **completed** compile (the `time_to_compile` sleep returned 0); an
  interrupted compile never counts. `compile_count == INT_MAX` → -1, unchanged.
  0 / -1.
- **7.2.1 `take_coder_dongles`**: validates input (`*taken` unchanged on -1);
  `first == second` → `take_single_dongle`, else `take_dongles`; on `taken == 1`
  prints `has taken a dongle` once per distinct dongle (twice, or once). On a log
  -1: `release_dongles` once (no retry); release -1 → -1; release 0 and a
  stop observed → `*taken = 0`, 0 (stop precedence); else -1. (Changed in the 7.2.1 spec: the function
  releases itself instead of returning `taken = 1` after a dropped line.)
- **7.3.1 `run_coder`** (`void *(*)(void *)`, returns NULL): loop until stop:
  take; not taken → leave; one coder → static wait (positive `sleep_ms` slices
  until stopped; never prints burnout), release, leave; else
  `set_compile_start`, `is compiling`, `sleep_ms(compile)`, `set_compile_count`,
  `release_dongles`, `is debugging`, `sleep_ms(debug)`, `is refactoring`,
  `sleep_ms(refactor)`; an `is_stopped` check closes each cycle (zero-length
  sleeps do not check stop). **One exit policy:** any -1 (setter, logger,
  sleep = stop or clock failure, release) → release what is held exactly once,
  then `set_stopped` + `wake_dongles` unconditionally (idempotent), leave.
  Lifecycle error reporting (failure vs normal end, process exit code) is
  recorded as an open point for phase 09.

## Resolved points

1. `compile_count` at completion (subject: completed compilations).
2. `sleep_ms` -1 is stop or failure: both take the single exit policy above.
3. Single-coder wait folded into `run_coder` (static helper).
4. Tests without monitor/main are enough: `init_simulation`, `start_ms` set
   before spawning, captured output drained concurrently, the test sets stop
   and wakes, joins and frees; synchronized observations, hang guards; fault
   branches by source review.
