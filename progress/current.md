# Current session

- **State (2026-11-02):** task ISS-14 is closed on its branch; ISS-14.1 is
  code-approved and its close artifacts and PR narrative are prepared. The
  harness will run the close check, commit, and open the task PR. The global harness gained the **issues lane**
  (`b7f97bb` + `3fdc195`, pushed) and codexion was migrated to the shared
  harness files (`install.sh . --migrate`, PR #16 merged, CI green):
  `Makefile` includes `harness/harness.mk`, `CLAUDE.md` imports
  `@harness/WORKFLOW.md`, `docs/plan/issues/README.md` turns the lane on.
  On `main`, `make check` green. Next phase work: 4.3.1 Push request.
- **Next:** after the task PR is opened and the user approves the merge, resume
  task 4.3 at subtask 4.3.1 (Push request), following its approved spec and
  usual plan/code/close gates.
- **Parked work:** none (the `wip-3.4.4` stash was dropped).
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

ISS-14.1 is APPROVED (`progress/review_ISS-14.1.md`). Its close artifacts and
task PR narrative are prepared; the harness runs the close check, commits the
task, pushes the branch, and opens the PR. The supplied pre-session `make check`
exited 0. After the user approves the task PR merge, return to 4.3.1. Every
subtask that edits `coders/Makefile` or `coders/main.h` must refresh the 42
header `Updated:` lines with Stdheader.

Note: `make check` does not run ThreadSanitizer or valgrind; CI does. PR #12
failed CI on `make tsan` (lock-order inversion created by the unit test
`tests/unit/test_print_burnout.c`, which locked `stop_mutex` before calling
`print_burnout`; production code was correct). Fixed post-close by the
implementer (`progress/fix_3.4_tsan.md`), committed by the user. From now on,
run `make tsan` and `make memcheck` (besides `make check`) before every code
gate, and ask the implementer to run them too.

Note: `make memcheck` and `make tsan` only run `tests/unit/`; acceptance
binaries that allocate (phase 04 on) are run explicitly under valgrind at the
code gate (`build/tests/acceptance/test_<id>.bin`).

Phase 04 breakdown (2026-10-29, Codex advisory): tasks 4.1 Request order,
4.2 Heap storage, 4.3 Heap operations; 7 subtasks 4.1.1-4.3.3, each approved
by the user, phase gate `PHASE_APPROVED` (`progress/phase_review_04.md`).
Invariants for phases 05-06: one
simulation-wide strictly increasing ticket per logical request (synchronized,
no overflow); the same `(key, ticket, coder)` pushed into both dongle heaps
under both mutexes (lower index first), unchanged while queued; check both
heads and availability and pop both in one critical section; check
`get_request_key` -1 before enqueueing; edf time base = ms elapsed since the
simulation start, `last_compile_start` 0 before the first compile; one coder
= one dongle, no double lock or push.

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
