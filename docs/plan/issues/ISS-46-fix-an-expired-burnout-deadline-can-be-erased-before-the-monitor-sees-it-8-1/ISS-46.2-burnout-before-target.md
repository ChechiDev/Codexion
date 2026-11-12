# ISS-46.2 Burnout before target
- status: pending
- layer: orchestration
- depends: ISS-46.1, 8.2.1
- files:
  - coders/src/monitor/run_monitor.c  # modifica: scan_step checks burnout before the compile target; update the scan-order comment
- migrates:
  - tests/acceptance/test_8.2.1.c
- acceptance:
  - AC1 | burnout wins a joint observation: synthetic state (no coder thread), config `4 300 200 200 200 3 50 fifo` built with `init_simulation`, every coder with `compile_count = 3` (global target met), index 0 with `last_compile_start = 0` and the others at `LONG_MAX`, `start_ms = get_time_ms() - 1000` (index 0 overdue); stdout captured by a draining reader thread; `run_monitor` runs in a thread joined within a 1000 ms hang guard: it returns NULL, exactly one line `<ts> 1 burned out\n` is captured, `sim.stopped == 1`, `sim.failed == 0` | tests/acceptance/test_ISS-46.2.c
  - AC2 | target alone still ends cleanly: same config, every coder with `compile_count = 3` and `last_compile_start = LONG_MAX` (nobody due): the monitor returns within the hang guard, nothing is captured (0 bytes), `sim.stopped == 1`, `sim.failed == 0` | tests/acceptance/test_ISS-46.2.c
  - AC3 | burnout alone unchanged: same config, index 3 at `compile_count = 2` (target not met), index 0 overdue as in AC1: exactly one line `<ts> 1 burned out\n`, `sim.stopped == 1` | tests/acceptance/test_ISS-46.2.c
  - AC4 | stopped by someone else still wins: AC1 state with `set_stopped(&sim)` called before the monitor starts: the monitor returns within the hang guard, nothing is captured, `sim.stopped` stays 1 | tests/acceptance/test_ISS-46.2.c
  - AC5 | zero required at the program boundary unchanged: `./coders/codexion 4 800 200 200 200 0 50 fifo` (through `CODEXION_BIN`) exits 0 with nothing on stdout or stderr | tests/acceptance/test_ISS-46.2.c
  - AC6 | `make -C coders` builds `codexion`, the binary defines `run_monitor` (`nm`), and `norminette coders/` reports no error | tests/acceptance/test_ISS-46.2.c

Migration of the sealed oracle (gate step, Codex-owned): in `tests/acceptance/test_8.2.1.c` only the two cases that pin target-before-burnout change. AC5 (`test_ac5_zero_target_precedes_burnout`, zero required with every deadline passed, a state that `run_simulation` never reaches because it returns before any thread when zero compiles are required, 9.4.1): it now expects exactly one `<ts> <id> burned out\n` line with `id` in 1..4 (no winner pinned, as AC2 of 8.2.1) and `sim.stopped == 1`. AC6 (`test_ac6_target_precedes_burnout_in_same_scan`): its first case now expects exactly one `<ts> 1 burned out\n` line and `sim.stopped == 1`; its counter-case (target not met) is unchanged. Function names and comments in that file are updated to the new rule. Every other case of `test_8.2.1.c` stays as it is. The migrated file keeps passing in `make check`.

Test notes for the oracle: at the code gate the leader also runs the acceptance binary under valgrind (`--leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42`, exit 0) and built with `-fsanitize=thread` under `setarch -R` (no `WARNING: ThreadSanitizer`). Every shared test flag is read and written under a test mutex.

Code review check: each scan, in this order: (1) `is_stopped` → leave silently; (2) the burnout scan through the existing single static helper (`get_burned_index`, lowest index tie-break unchanged) → `print_burnout`, leave; (3) `get_target_met` (0 required included) → `set_stopped` + `wake_dongles`, leave; (4) `usleep(1000)`; every exit path, `set_failed` on error and the wake-up of every waiter are unchanged; `get_burned_out.c`, `get_target_met.c`, `print_burnout.c` and every coder file are not modified; the scan-order comment in `run_monitor.c` states the new order and why; `tests/unit/test_run_monitor.c` is updated if it pins the old order; the sealed spec of 8.2.1 is not edited; the 42 header `Updated:` line of `run_monitor.c` is refreshed with Stdheader; Norm respected.

## Description

GitHub issue #46, second part. `scan_step` in `run_monitor.c` checks the
compile target before the burnout scan (8.2.1, user decision 2026-11-05).
A coder can finish its quota, reach its deadline while others are still
compiling, and another coder can complete the last outstanding compile before
the next scan; the monitor then sees both in one pass and leaves silently,
although the burnout happened first. Subject: a coder burns out when it does
not start compiling within `time_to_burnout` (line 133), the monitor must
detect it precisely and stop the run (line 209), and log it within 10 ms
(line 160).

**New scan order:** stopped → burnout → target. When both are observed in the
same pass, the burnout is reported. Neither order can reconstruct which event
came first; burnout-first is the conservative policy, it never hides a reached
deadline (Codex advisory `progress/advisory_ISS-46.2_order.md`, user decision
2026-11-11).

**Supersedes.** In 8.2.1: the "target checked before burnout" rule and its
AC5 and AC6 (first case). The sealed spec stays as the historical record; only
the acceptance test is migrated (`migrates:`).

**Zero required.** Unaffected at the program boundary: `run_simulation`
returns 0 before creating any thread (9.4.1), so the monitor never sees that
case in a real run (AC5).

**Name.** No new identifier; no prototype change.

**README.** No update: it already says the run stops when a coder burns out or
every coder reached the target, with no precedence stated.
