# Phase 09 — Simulation lifecycle
- goal: main wires parse, init, start the coder and monitor threads, join and teardown end to end, with clean shutdown on pthread_create failure and the edge cases (1 coder, zero compiles required) behaving as specified

## 9.1 Failure state
- layer: adapters
- goal: `t_sim` gains a `failed` flag (cleared by `init_simulation`) and the monitor thread handle; `set_failed` marks a runtime failure under `stop_mutex` without stopping the run (stopping stays `set_stopped`'s job), so the lifecycle can tell a normal end (burnout or target met) from an internal error

## 9.2 Report runtime failures
- layer: orchestration
- goal: a coder marks `failed` only on a real error (a -1, never on the `sleep_ms` stop result 1 from ISS-32), and the monitor separates a clean target-met stop from an error exit (failed check, burnout line, `set_stopped`, `wake_dongles` or `usleep` failure) that marks `failed`, stops and wakes every waiter; a logged burnout stays a normal end

## 9.3 Threads
- layer: orchestration
- goal: start the monitor first and then the N coders right after setting `start_ms` (no start barrier, user decision 2026-11-06); on any `pthread_create` failure mark `failed`, stop, wake every waiter and join exactly the threads created (none when the monitor itself failed, or when the clock read fails before any creation); join the monitor and the coders trying every join and reporting any failure

## 9.4 Lifecycle
- layer: interfaces
- goal: `run_simulation` returns 0 at once with no threads and no output when `number_of_compiles_required` is 0 (user decision 2026-11-06), otherwise init → start → join → read `failed` → `free_sim` (no `free_sim` after a failed init, no free after a failed join), 0 on a normal end and -1 on any failure; `main` runs it after validation and on -1 prints an `Error:` line to stderr and exits with a runtime status distinct from `EXIT_USAGE`
