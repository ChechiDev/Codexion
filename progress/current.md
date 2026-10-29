# Current session

- **Current subtask:** 3.4.4 Print burnout (replanned; last subtask of task
  3.4 and of phase 03), `done` on branch `feat/3.4-serialized-logger`.
  Phase: closed (`APPROVED`; close documentation and tracking prepared). 3.4.5
  (`1e49f64`) and 3.4.6 (`c3058f9`) closed.
- **Next:** harness runs the close check and commits task 3.4; then break down
  phase 04.
- **Parked work:** the first 3.4.4 implementation (`print_burnout.c`, its
  `main.h`/`Makefile` edits, `test_3.4.4.c`, `test_print_burnout.c`,
  `progress/acceptance_3.4.4.sha256`) is in `git stash` as `wip-3.4.4`; the
  user deleted the stale `coders/src/log/print_burnout.o`.
- **Closed in task 3.4:** 3.4.1 (`7635017`), 3.4.2 (`8977acd`), 3.4.3 (`4a60ab4`).
- **Previous:** task 3.3 done and merged (PR #11, merge commit `f065027`).
  Tasks 3.1 and 3.2 merged (PRs #9, #10). Phase 03 is `PHASE_APPROVED`.
- **Start:** 2026-10-28
- **Engine/role:** leader → phase gate (Codex) → user OK → per-subtask cycle

## Decisions taken during the phase 03 breakdown (2026-10-28)

- PR #8 (`style/threading-conventions`, merged `4d569c5`): `main.h` may include
  `<pthread.h>`; new verbs `sleep_`, `wait_`, `take_`/`release_`,
  `push_`/`pop_`, `sift_`, `start_`/`join_`, `run_`.
- Logger: one unbuffered `write` per line instead of `printf` (`PLAN.md` "Log";
  roadmap re-gated `ROADMAP_APPROVED`), so the burnout line is visible at once
  on a pipe. Numbers formatted by `build_number_text` (3.4.1).
- Justified exceptions to the 0/-1 + out-parameter convention: `get_time_ms`,
  `get_elapsed_ms` (value or -1), `is_stopped` (lock failure → 1),
  `build_number_text` and `build_log_line` (length or -1).
- 3.2.1 migrates the sealed oracle `tests/acceptance/test_2.2.3.c` AC4 (it
  forbade any system include in `main.h`) at its plan gate, as 2.2.7 did.

## Harness change (global, already pushed)

`~/.claude-codex-harness` commit `54ca858` (user-granted exception for
`lib/_pr.py` and `lib/pr_template.md` only): the task PR "Files" table now lists
only code, tests, CI, build files and README.md, and a new "Documentation"
section points to the task's spec folder. Applies from the task 2.3 PR on.

## Next step

The 3.4.4 close is prepared. The harness runs the close check, commits the task
branch, and opens the task 3.4 PR; then break down phase 04. Every subtask that edits `coders/Makefile` or `coders/main.h` must
refresh the 42 header `Updated:` lines with Stdheader.

Note: `make check` does not run ThreadSanitizer or valgrind; CI does. PR #12
failed CI on `make tsan` (lock-order inversion created by the unit test
`tests/unit/test_print_burnout.c`, which locked `stop_mutex` before calling
`print_burnout`; production code was correct). Fixed post-close by the
implementer (`progress/fix_3.4_tsan.md`), committed by the user. From now on,
run `make tsan` and `make memcheck` (besides `make check`) before every code
gate, and ask the implementer to run them too.

Note: isolated, non-reproducible test failures (3.3.1 run, `test_3.1.2.c`
after the 3.4.5 plan gate, `test_1.1.1.c` "coders/codexion missing" during
3.4.5) are explained by concurrent `make` runs: the stop hook runs
`make check` at the end of each leader turn while a background implementer
is building, and `test_1.1.1.c` runs `make -C coders fclean`. None reproduced
in sequential runs (200 direct runs of 3.1.2, repeated full `make check`).
Always re-run `make check` sequentially before trusting a red result.

Note: avoid running two `make` processes at once; acceptance tests call
`make -C coders` themselves and a concurrent build can make them fail
spuriously (seen once during 3.3.1).

Note: agents cannot delete files under `coders/` (the guard blocks shell `rm`
and there is no delete tool). Deletions declared by a spec and stale build
objects of moved sources are done by the user by hand.
