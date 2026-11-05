# Architecture (C)

> **Binding subject:** `docs/codexion-rules.md` (42 Codexion). Everything here
> applies only where it does not contradict it; on conflict the subject wins.

> What "good work" means in this repo. The hexagonal idea still applies, adapted
> to C: dependencies point toward the core; I/O and platform code sit at the edges.

## Layers (dependencies point inward)

- **core** — pure logic and data structures. No I/O, no syscalls, no globals.
  Deterministic, trivially testable.
- **adapters** — I/O, time, threads and mutexes. They depend on core, never the
  other way round.
- **interfaces** — the entry point: `main` and what it prints to the user.

The project has a single header (`coders/main.h`), so the layers are kept by
discipline, not by includes: a core function never calls `write`, `printf`,
`gettimeofday` or a `pthread_*` function.

## Modules (`coders/src/`)

| Folder | Layer | Owns | Depends on |
|---|---|---|---|
| `utils/` | core | generic helpers with no domain knowledge (`is_space`, `is_digit`, `trim_slice`) | nothing |
| `parsing/` | core | turning `argv` text into a validated `t_config` | `utils/` |
| `cli/` | interfaces | every user-facing message: the usage text (`print_usage`) and the argument errors | `parsing/` (minimums) |
| `state/` | adapters | shared simulation state guarded by its mutexes, including the synchronized request-ticket counter | nothing |
| `time/` | adapters | clock readings for elapsed simulation time and absolute deadlines for `pthread_cond_timedwait` (`get_time_ms`, `get_elapsed_ms`, `get_deadline`) | system clock |
| `init/` | adapters | lifecycle setup for simulation state, including coder mutexes, dongle mutexes, condition variables, and request heaps; `init_simulation` builds the state in dependency order and uses `free_sim` for rollback | core heap and seat-mapping APIs; cleanup lifecycle APIs |
| `cleanup/` | adapters | lifecycle teardown for simulation resources, including coder mutexes and arrays, dongle arrays and their heaps, condition variables, and mutexes; `free_sim` releases the full simulation in reverse initialisation order | core heap API |
| `log/` | core | pure construction of serialized log-line bytes (`build_log_line`) | `utils/` |
| `policy/` | core | pure FIFO/EDF request-key calculation (`get_request_key`), ordering (`is_request_before`), and the read-only dongle readiness predicate (`is_dongle_ready`) | `heap/` request head API |
| `heap/` | core | caller-owned array storage for the hand-written binary min-heap (`init_heap`, `free_heap`, then push/peek/pop) | `policy/` ordering contract |
| `dongle/` | orchestration | builds a coder's FIFO/EDF request from synchronized ticket allocation and the pure request-key policy (`build_request`); acquires a pair atomically or the single dongle for one coder (`take_dongles`, `take_single_dongle`); releases a coder's dongles at the actual release time, starts their cooldown, and wakes waiters (`release_dongles`); broadcasts stop wake-ups across dongle condition variables (`wake_dongles`) | `state/`, `policy/`, `time/` |
| `coder/` | orchestration | updates per-coder compile state under that coder's mutex (`set_compile_start`, `set_compile_count`); acquires dongles and logs each distinct acquisition (`take_coder_dongles`) | `time/`, `dongle/`, `log/` |
| `main.c` | interfaces | the entry point only: wires the modules together | every module |

Later phases add their own folders (time and log, queue, simulation, ...) and
this table grows with them. Rules for each folder, `main.h` sections and names
are in `docs/conventions.md`.

## Memory & ownership

- Every allocation has a documented owner and a single free path.
- Prefer views over copies: `t_slice` points into a string it does not own, so
  it needs no allocation and never modifies its source (`argv`).
- Prefer caller-allocates / callee-fills; document the contract in the spec.
