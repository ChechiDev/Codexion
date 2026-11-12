# ISS-46.1 Keep expired deadline
- status: done
- layer: orchestration
- depends: 7.1.1, 7.3.1, 8.1.1
- files:
  - coders/src/coder/set_compile_start.c  # modifica: return 1 and keep the field when the current deadline has already been reached; update the contract comment
- migrates:
  - tests/acceptance/test_7.1.1.c
- acceptance:
  - AC1 | deadline not reached: on a simulation built with `init_simulation` (config `4 800 200 200 200 3 50 fifo`) whose `start_ms` the test sets to `get_time_ms() - 500`, with the coder at index 1 prefilled with `compile_count = 7` and `last_compile_start = 100` (deadline 900, not reached): `set_compile_start(&sim.coders[1])` returns 0 and the field lies between `get_elapsed_ms(start_ms)` read right before and right after the call; `id`, `first_dongle`, `second_dongle` and `compile_count` (7) are unchanged; the coder mutex is unlocked afterwards (`pthread_mutex_trylock` succeeds, then unlocked); `free_sim` returns 0 | tests/acceptance/test_ISS-46.1.c
  - AC2 | deadline already passed: same config, `start_ms = get_time_ms() - 1000`, coder at index 1 prefilled with `compile_count = 7` and `last_compile_start = 0` (deadline 800, passed): the call returns 1 (not 0, not -1); the field is still 0, `compile_count` still 7; the coder mutex is unlocked afterwards; `sim.stopped` and `sim.failed` are still 0 (read under their mutexes); nothing is written to stdout or stderr | tests/acceptance/test_ISS-46.1.c
  - AC3 | the deadline is inclusive (same rule as `get_burned_out`, `last <= now - time_to_burnout`): with `start_ms = get_time_ms() - 1000` and the field prefilled with 200 (deadline exactly 1000, reached at the call since the clock only advances), the call returns 1 and the field is still 200; with the field prefilled with 400 (deadline 1200, not reached), the call returns 0 and stores the current elapsed time (assumes the wall clock is not set backwards and the call takes under 200 ms) | tests/acceptance/test_ISS-46.1.c
  - AC4 | the kept deadline is still visible to the monitor: right after the AC2 call returned 1, `get_burned_out(&sim.coders[1], &burned)` returns 0 with `burned == 1` | tests/acceptance/test_ISS-46.1.c
  - AC5 | caller behaviour through `run_coder`: a simulation built with `init_simulation` from `2 800 200 200 200 3 0 fifo`, stdout captured by a draining reader thread; `start_ms = get_time_ms() - 1000` (deadline of coder 1 already passed), no monitor thread; one `run_coder` thread on coder 1, joined within a 1000 ms hang guard: it returns NULL; no `is compiling` line for coder 1 is captured; `compile_count` of coder 1 is 0 and its `last_compile_start` is still 0; both of its dongles are available again (`available == 1`); `sim.stopped` and `sim.failed` are still 0; `get_burned_out` on coder 1 then reports `burned == 1`; every mutex is unlocked and `free_sim` returns 0 | tests/acceptance/test_ISS-46.1.c
  - AC6 | unchanged paths: NULL coder and a coder whose `sim` is NULL return -1 with nothing changed; with `start_ms = LONG_MAX` (clock clamps to 0) and the field prefilled with 100 (deadline 900, not reached), the call returns 0 and stores 0 | tests/acceptance/test_ISS-46.1.c
  - AC7 | `make -C coders` builds `codexion`, the binary defines `set_compile_start` (`nm`), and `norminette coders/` reports no error | tests/acceptance/test_ISS-46.1.c

Migration of the sealed oracle (gate step, Codex-owned): `tests/acceptance/test_7.1.1.c` prefills `last_compile_start = -12345` in AC1, AC3 and AC4, which is an already-reached deadline under the new contract, so those calls would now return 1. Only those prefill sentinels change, to a value whose deadline is not reached at the call and that differs from the value the call stores (for example 100, with `start_ms = get_time_ms() - 500`, or with `start_ms = LONG_MAX` in AC3); every assertion keeps its meaning (AC1 stores the current time, AC3 clamps to 0, AC4 blocks while the mutex is held and the field keeps its prefill until the unlock). AC2 and AC5 are not touched. The migrated file keeps passing in `make check`.

Test notes for the oracle: at the code gate the leader also runs the acceptance binary under valgrind (`--leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42`, exit 0) and built with `-fsanitize=thread` under `setarch -R` (no `WARNING: ThreadSanitizer`). Clock and lock/unlock failures stay without AC (not forceable deterministically): code review.

Code review check: the deadline check uses the same inclusive rule as `get_burned_out` (`last_compile_start <= now - time_to_burnout`, `time_to_burnout` from `sim->config`), with `now` read by `get_elapsed_ms(sim->start_ms)` **while holding** the coder mutex, so the check and the write are one critical section and the monitor can never read a start that hides a reached deadline; when the deadline is reached the field is not written and the function unlocks and returns 1; a lock failure, a clock failure or an unlock failure still return -1 (an unlock failure wins over 1); NULL `coder` or `coder->sim` returns -1 before any lock; only the coder's own mutex is taken; no log, sleep, stop or failure flag is set by `set_compile_start`; the contract comment states 0 / 1 / -1; `run_coder.c` is **not** modified: `sleep_compile` returns the 1, the routine loop ends and `release_run` releases the held dongles without `set_failed` or `set_stopped` (same path as a normal stop, AC5 is the regression check); `get_burned_out.c` and `run_monitor.c` are not modified (the target-versus-burnout order is ISS-46.2); `tests/unit/test_set_compile_start.c` is updated to the new contract; the sealed spec of 7.1.1 is not edited; the 42 header `Updated:` line of `set_compile_start.c` is refreshed with Stdheader; Norm respected.

## Description

GitHub issue #46, first part. `int set_compile_start(t_coder *coder)` (7.1.1)
overwrites `last_compile_start` unconditionally. The monitor scans about
every 1 ms and only sees that field, so a coder whose deadline was reached
just after a scan, and who takes its dongles before the next scan, erases the
reached deadline: the burnout is never logged and the run goes on, against
subject §5.2 (`time_to_burnout`) and §6 rules 7 and 9.

**New return contract (follows `docs/conventions.md`, same 0 / 1 / -1 shape as
`sleep_ms` and `print_state`).**
- 0: the deadline was not reached; the current elapsed time was stored.
- 1: the deadline `last_compile_start + time_to_burnout` was already reached
  (inclusive, as in `get_burned_out`); the field is left as it is so the
  monitor detects and logs the burnout; the coder must not compile.
- -1: NULL input, lock, clock or unlock failure (unchanged).

**Supersedes.** In 7.1.1: the "always overwrite" part of the contract and the
`-12345` prefill of its AC1, AC3 and AC4 (migrated test only; the sealed spec
stays as the historical record).

**Caller.** `run_coder` (7.3.1) already treats any non-zero result of
`sleep_compile` as the end of the routine and releases the dongles through
`release_run`; a 1 there does not mark `failed` nor stop the simulation, so
the monitor stays the only one that stops for a normal reason (9.2.3
invariant). No caller change.

**Name.** No new identifier; signature and prototype unchanged.

**README.** No update (behaviour already described: a coder that cannot start
compiling within `time_to_burnout` burns out).
