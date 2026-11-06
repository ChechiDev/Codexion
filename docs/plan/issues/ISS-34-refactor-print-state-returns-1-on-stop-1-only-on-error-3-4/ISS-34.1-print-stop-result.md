# ISS-34.1 Print stop result
- status: done
- layer: adapters
- depends: 3.4.3, 3.4.6, 7.2.1, 7.3.1
- files:
  - coders/src/log/print_state.c  # modifica: return 1 when the simulation is stopped, keep -1 for every failure; update the contract comment
- migrates:
  - tests/acceptance/test_3.4.3.c
  - tests/acceptance/test_3.4.4.c
  - tests/acceptance/test_3.4.5.c
  - tests/acceptance/test_3.4.6.c
- acceptance:
  - AC1 | running simulation: on a `t_sim` built by `init_simulation` (config `4 800 200 200 200 3 50 fifo`), stdout captured, each of the four states with coder 1 returns 0 and writes exactly one line `<ts> 1 <text>\n` with the subject text; nothing on stderr; afterwards `log_mutex` and `stop_mutex` are unlocked and `stopped` is still 0 | tests/acceptance/test_ISS-34.1.c
  - AC2 | stopped simulation: after `set_stopped(&sim)` returns 0, each of the four states returns 1 (not -1) and nothing is written to stdout or stderr; afterwards both mutexes are unlocked | tests/acceptance/test_ISS-34.1.c
  - AC3 | concurrent stop: 4 threads each make one successful call (0), then loop on `print_state` counting their 0 returns until a call returns non-zero; the main thread calls `set_stopped` (returns 0) once every thread made its first call; every thread's loop ends with a 1 (never -1), and after the joins the number of captured lines equals the total number of 0 returns; every captured line is a complete `<ts> <id> <text>\n` | tests/acceptance/test_ISS-34.1.c
  - AC4 | failures stay -1, on a running simulation: an invalid state (`-1`, `4`) with a valid coder and a coder below 1 (`0`, `-3`) with a valid state each return -1 and write nothing; with `log_mutex` as `PTHREAD_MUTEX_ERRORCHECK` already held by the caller, -1 and nothing written; with `stop_mutex` held that way, -1, nothing written and `log_mutex` unlocked again (test-built `t_sim` for the lock cases, as in 3.4.3); in a child process whose stdout is `/dev/full`, a call on a running simulation returns -1 | tests/acceptance/test_ISS-34.1.c
  - AC5 | rejected input wins over the stop: on a stopped simulation an invalid state (`-1`, `4`) with a valid coder and a coder below 1 (`0`, `-3`) with a valid state each still return -1 (never 1) and write nothing; afterwards both mutexes are unlocked | tests/acceptance/test_ISS-34.1.c
  - AC6 | callers unchanged, through their public API: (a) a simulation built with `init_simulation` from `4 800 200 100 100 50 50 fifo`, stdout captured by a draining reader thread; `start_ms = get_time_ms()`, four `run_coder` threads; after 700 ms the test records `t`, calls `set_stopped` (0) and `wake_dongles`; every coder thread returns NULL, the last join at most `t + 250 + S` ms; the sum of `compile_count` is at least 2; every captured line is `<ts> <id> <text>` with `id` in 1..4 and one of the four state texts, timestamps non-decreasing, no `burned out` line; every coder mutex, `stop_mutex` and `log_mutex` unlocked; `free_sim` returns 0; (b) on a simulation already stopped before the call, `take_coder_dongles(&sim.coders[0], &taken)` returns 0 with `taken == 0`, writes nothing, and leaves both dongles of coder 1 available (`available == 1`); heap contents are not checked: on a stopped simulation `take_dongles` returns without popping the request it queued (sealed 6.3.4 contract, "stopped → unlock, `*taken = 0`, return 0"), which is harmless because the run is over and `free_sim` frees the heaps (oracle fix 2026-11-06, see `progress/current.md`) | tests/acceptance/test_ISS-34.1.c
  - AC7 | `make -C coders` builds `codexion`, the binary defines `print_state` (`nm`), and `norminette coders/` reports no error | tests/acceptance/test_ISS-34.1.c

`S` is the timing slack in ms read from `CODEXION_TIME_SLACK_MS` (default 0; CI exports 40), added to upper bounds only.

Migration of the sealed oracles (gate step, Codex-owned), only the assertions and loop conditions that depend on a stop returning -1 change to 1, everything else in those files stays as it is: `tests/acceptance/test_3.4.3.c` (AC4 post-stop loop near line 255; AC7 worker loop stop condition near line 436 and post-stop assertion near line 445); `tests/acceptance/test_3.4.4.c` (post-stop loop near line 294; worker loop stop condition near line 410; post-stop assertion near line 420); `tests/acceptance/test_3.4.5.c` (post-stop loop near line 69); `tests/acceptance/test_3.4.6.c` (post-stop assertion near line 278). The -1 assertions for rejected input, lock failures and write failures in those files are not touched. Each migrated file keeps passing in `make check`.

Code review check: the behavioural changes in `print_state.c` are (1) the value returned when the flag read under `stop_mutex` shows the simulation stopped (1 instead of -1) and (2) a coder below 1 is now rejected with -1 before any lock, next to the existing invalid-state check (today it is only rejected later by `build_log_line`, after the stop read, so without this move a stopped simulation would turn an invalid coder into 1); `build_log_line` is unchanged and keeps its own check; lock order `log_mutex → stop_mutex` and every unlock attempt are unchanged; a `stop_mutex` lock or unlock failure while reading the flag stays -1 and writes nothing; clock, build and `write` failures stay -1 (clock and build failures not forceable: inspection); a `log_mutex` unlock failure after a stopped read returns -1 (failure wins over the stop result); the contract comment above `print_state` states 0 / 1 / -1; `run_coder.c` and `take_coder_dongles.c` are **not** modified (their four uses test `!= 0`; in `take_coder_dongles` a 1 goes through `release_taken` exactly like a -1 today); `print_burnout` and `print_log_line` are not modified; `tests/unit/test_print_state.c` is updated to the new contract (stopped case expects 1); the sealed specs of 3.4.3 to 3.4.6 are not edited; the 42 header `Updated:` line of `print_state.c` is refreshed with Stdheader; Norm respected.

## Description

GitHub issue #34. `int print_state(t_sim *sim, int coder, t_state state)`
(3.4.3) returns -1 both when it suppresses a line because the simulation is
stopped and on every real failure, so `run_coder` cannot tell a normal end
from an error. Phase 09 needs that distinction (9.2.1 marks `failed` only on
a real error), and issue #32 already gave `sleep_ms` the same three-way
contract.

**New return contract (follows `docs/conventions.md`).**
- 0: the line was written with one `write` of the full length.
- 1: the simulation was stopped (flag read under `stop_mutex` while holding
  `log_mutex`); nothing written.
- -1: any failure: invalid state or coder (both checked before any lock, so
  rejected input is -1 whether or not the simulation is stopped), `log_mutex` or `stop_mutex` lock or
  unlock failure, clock, build or `write` failure. If a failure and a stopped
  read happen in the same call (for example the final `log_mutex` unlock
  fails), -1 wins.

**Supersedes.** In 3.4.3: AC4 and the post-stop expectation of AC7; in 3.4.4,
3.4.5 and 3.4.6: the `print_state`-after-stop expectations of their sealed
tests (listed in the migration paragraph). Their sealed specs stay as the
historical record; only the acceptance tests are migrated (`migrates:`).

**Callers.** `run_coder` (7.3.1) and `take_coder_dongles` (7.2.1) are the only
production callers and need no change (AC6 is the regression check). Using
the new 1 to avoid marking `failed` on a normal stop is phase 09 work
(9.2.1).

**Name.** No new identifier; `print_state` keeps its signature and prototype.

**README.** No update (no user-visible change).
