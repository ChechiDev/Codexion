# ISS-32.1 Sleep stop result
- status: done
- layer: adapters
- depends: 3.3.1, 7.3.1
- files:
  - coders/src/time/sleep_ms.c  # modifica: return 1 when the stop is observed, keep -1 for clock failures
  - coders/main.h               # modifica: refresh the 42 header only if a Time section comment is added (signature unchanged)
- migrates:
  - tests/acceptance/test_3.3.1.c
- acceptance:
  - AC1 | `ms == 0` and `ms < 0` (including `LONG_MIN`) return 0 in under `2 + S` ms, on a running and on a stopped `t_sim` | tests/acceptance/test_ISS-32.1.c
  - AC2 | on a running `t_sim`, `sleep_ms(sim, 100)` returns 0; `get_time_ms()` read right before and right after the call differ by at least 100 and at most `110 + S` ms | tests/acceptance/test_ISS-32.1.c
  - AC3 | on an already stopped `t_sim`, `sleep_ms(sim, 1000)` returns 1 (not -1) in under `2 + S` ms; the same holds for `sleep_ms(sim, LONG_MAX)` | tests/acceptance/test_ISS-32.1.c
  - AC4 | stop response: a worker thread signals it is ready and runs `sleep_ms(sim, 10000)` (longer than the test's 5 s watchdog); about 50 ms later the main thread records `t = get_time_ms()` and calls `set_stopped`, which returns 0; the worker's call returns 1, and its return time is at least `t` and at most `t + 10 + S` ms | tests/acceptance/test_ISS-32.1.c
  - AC5 | coder behaviour unchanged, through `run_coder`'s public API (no `main` involved): a simulation built with `init_simulation` from config `4 800 200 100 100 50 50 fifo` (target not reached during the run), stdout captured to a pipe drained by a reader thread; the test sets `start_ms = get_time_ms()` and starts `run_coder(&sim.coders[i])` in four threads; after 700 ms it records `t = get_time_ms()`, calls `set_stopped(&sim)` (returns 0) and `wake_dongles(&sim)`; every coder thread returns NULL and is joined, the last join at most `t + 250 + S` ms (liveness budget: a coder may be inside a 200 ms compile, which `sleep_ms` interrupts); the sum of `compile_count` over the four coders is at least 2, so the criterion cannot pass with no coder running and sleeping; every captured line is `<ts> <id> <text>` with `id` in 1..4 and `text` one of `has taken a dongle`, `is compiling`, `is debugging`, `is refactoring`, timestamps non-decreasing, no `burned out` line; after the joins every coder mutex, `stop_mutex` and `log_mutex` are unlocked (`pthread_mutex_trylock` succeeds, then unlocked) and `free_sim` returns 0 | tests/acceptance/test_ISS-32.1.c
  - AC6 | `make -C coders` builds `codexion`, the binary defines `sleep_ms` (`nm`), and `norminette coders/` reports no error | tests/acceptance/test_ISS-32.1.c

`S` is the timing slack in ms read from `CODEXION_TIME_SLACK_MS` (default 0; CI exports 40), added to upper bounds only, as in 3.3.1. The timing ACs assume no backward wall-clock adjustment during the test.

Migration of `tests/acceptance/test_3.3.1.c` (gate step, Codex-owned): its AC3 and AC4 assertions change from -1 to 1 on a stop; AC1, AC2 and AC5 stay unchanged; it keeps passing in `make check`.

Code review check: the only behavioural change in `sleep_ms.c` is the value returned when `is_stopped(sim)` reports a stop (1 instead of -1); every clock failure (initial `get_time_ms` and the elapsed read in the loop) still returns -1; the stop is still checked before the elapsed time on each iteration; `ms <= 0` still returns 0 before any clock, mutex or `usleep` call; slices, overflow cap and the "failed `usleep` continues" rule of 3.3.1 are unchanged; `coders/src/coder/run_coder.c` is **not** modified and its four uses of `sleep_ms` keep their behaviour (lines 36 and 51 test `!= 0`; `sleep_rest` returns the value and its caller tests `!= 0`; the single-coder wait loops while `== 0`), so 1 and -1 still both make the coder leave; no other production file calls `sleep_ms`; `tests/unit/test_sleep_ms.c` is updated to the new contract (the `LONG_MAX` stopped case expects 1); the sealed `docs/plan/phase-03-time-and-log/3.3.1-sleep-ms.md` is not edited; the 42 header `Updated:` line of every modified production file is refreshed with Stdheader; Norm respected.

## Description

GitHub issue #32. `int sleep_ms(t_sim *sim, long ms)` (3.3.1) returns -1 both
when it observes the stop and when the clock fails, so a caller cannot tell a
normal end of the simulation from a real error. Phase 09 needs that
distinction: a synchronized `failed` flag will make runtime failures exit
non-zero, and a coder interrupted in its sleep by a normal stop (burnout or
target met) must not count as a failure.

**New return contract (follows `docs/conventions.md`).**
- 0: the duration was reached (project-clock milliseconds), or `ms <= 0`.
- 1: the stop was observed. This includes a lock or unlock failure of
  `stop_mutex` inside `is_stopped`, which reports "stopped" (sealed 3.2.1 AC3);
  that failure stays indistinguishable from a stop, an accepted limit already
  documented in 8.2.1, because it cannot be forced with ordinary inputs and
  changing `is_stopped` would migrate a second sealed oracle and every caller.
  As before, the stop is checked first, so 1 is possible even if the duration
  had just elapsed.
- -1: the clock failed (`get_time_ms` returned -1, 3.1.1). Not forceable
  without fault injection: code review.

**Supersedes.** AC3 and AC4 of 3.3.1 (they expected -1 on a stop). The sealed
3.3.1 spec stays as the historical record; only its acceptance test is
migrated (`migrates:`). AC1 to AC4 here restate the 3.3.1 timing criteria with
the new value so the contract is pinned in this issue's own oracle.

**Callers.** `run_coder` (7.3.1) is the only production caller and needs no
change: every use reacts to any non-zero result by leaving, so the coder
behaves exactly as before (AC5 is the regression check through `run_coder`, and the sealed 7.3.1 oracle keeps running in `make check`; full-binary runs come once phase 09 wires `main`). Using
the new 1 to avoid marking `failed` on a normal stop is phase 09 work (9.2.1).

**Name.** No new identifier; `sleep_ms` keeps its signature and prototype.

**README.** No update (no user-visible change).
