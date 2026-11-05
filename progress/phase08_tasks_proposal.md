# Phase 08 — task breakdown proposal (pre-gate)

Status: proposal by the leader, **not yet approved by the user**. Specs are
written one at a time after approval; then the formal phase gate
(`review_with_codex.sh phase 08`) runs.

Phase goal (sealed, `docs/plan/phase-08-monitor.md`): a separate monitor
thread that detects a burnout and logs it within 10 ms, or detects that every
coder reached `number_of_compiles_required`, then sets stop and wakes all
waiting threads.

Inputs already built: `t_coder {mutex, last_compile_start, compile_count}`,
`get_elapsed_ms`, `is_stopped`, `set_stopped`, `print_burnout(sim, coder)`
(first stop + line in one critical section under `log_mutex`; -1 if already
stopped), `wake_dongles`, `sleep_ms`. Names below checked: no clash in
`tests/` or `coders/`.

## Tasks (v2, after the Codex advisory `progress/phase08_tasks_advisory.md`)

| Task | Layer | Subtasks |
|---|---|---|
| 8.1 Monitor checks | orchestration | 8.1.1 `get_burned_out(t_coder *coder, int *burned)` · 8.1.2 `get_target_met(t_sim *sim, int *met)` |
| 8.2 Monitor routine | orchestration | 8.2.1 `run_monitor(void *arg)` |

3 micro-subtasks, 1-3 files each (`.c` + `main.h` + `coders/Makefile`), new
module `coders/src/monitor/`.

## Contracts (v2)

- **8.1.1 `get_burned_out`**: validates `coder`, `coder->sim`, `burned`
  (`*burned` unchanged on -1); under the coder mutex reads
  `last_compile_start` **and** `get_elapsed_ms(sim->start_ms)` (clock -1 →
  unlock, -1); `*burned = (now - last_compile_start >= time_to_burnout)`
  (Q1: `>=`), published only after a successful unlock. A coder that met its
  own target can still burn out. 0 / -1. Never logs.
- **8.1.2 `get_target_met`**: validates `sim`, `sim->coders`,
  `number_of_coders > 0`, `met`; required 0 → `*met = 1` at once; else reads
  each `compile_count` under its own coder mutex, one at a time (counters
  only grow, so the snapshot is sound). `*met` unchanged on any error,
  including an unlock failure. 0 / -1.
- **8.2.1 `run_monitor`** (`void *(*)(void *)`, arg `t_sim *`, returns NULL;
  NULL arg → return NULL; requires `start_ms` set before the thread starts):
  each scan: (1) target check first only when required is 0 (0 wins, stop
  without burnout); (2) for each coder `get_burned_out` (after the target check, see below); burned →
  `print_burnout(sim, id)` at once (no finishing the scan), then
  `wake_dongles`, leave; (3) `get_target_met`; met → `set_stopped`,
  `wake_dongles`, leave; (4) stopped by someone else → leave silently;
  (5) `usleep` ~1 ms. Never `set_stopped` before `print_burnout` (its -1 also
  means "already stopped"). Any check -1 → `set_stopped` + `wake_dongles`
  (each tried even if the previous failed, no retries), leave. No wake while
  holding a coder/log/stop mutex. 1 coder: no special case. Note: 1 ms sleep
  plus scan plus `write` is the latency budget; the ≤ 10 ms guarantee assumes
  consumed output and normal scheduling.

## Acceptance (sketch)

See advisory point 6: controlled deadlines (-1/0/+1), barrier-driven
concurrent renewal, fault injection with outputs untouched, real monitor on an
initialised sim with exactly one terminal line, woken waiters, bounded joins,
pipe-reception latency ≤ 10 ms, TSan. Full-binary runs wait for phase 09.

- **8.2.1 scan order changed (2026-11-05, user decision after `progress/advisory_8.2.1_ac6.md`):** stopped → target met (0 included) → burnout → sleep. Tie-break between burned coders kept local to one helper (Defense note in the spec).
