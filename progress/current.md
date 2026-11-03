# Current session

- **State (2026-11-03):** issue #14 done (ISS-14.1, PR #17 merged). Task 4.3
  done: 4.3.1 Push request, 4.3.2 Get head request, 4.3.3 Pop request; PR #18
  squash-merged into `main` (`f43299e`). **Phase 04 Priority queue is done
  (7/7).** On `main`, `make check` green, tree clean.
- **Phase 05 breakdown:** tasks 5.1-5.4 approved by the user after a Codex
  advisory review (`progress/phase05_tasks_proposal.md`, `..._advisory.md`);
  11 specs 5.1.1-5.4.2 validated one by one (free_* before init_* in 5.2-5.4
  so init rolls back through them). Phase gate `PHASE_APPROVED` on round 4
  (`progress/phase_review_05.md`; rounds asked to drop the `nm` and build/Norm
  ACs, now in "Code review check", and to add depends 5.2.1→5.1.1,
  5.3.1→5.2.4). User OK given.
- **Now:** tasks 5.1 (PR #19) and 5.2 (PR #20) merged. 5.3.1 closed (`0350ecf`)
  and 5.3.2 Destroy coder is closed on `feat/5.3-coders`; plan and code gates
  are approved, and the supplied outside-sandbox close `make check` exited 0.
  The harness will open the task PR after committing this close.
- **Merge style:** the user squash-merged #18; recommended default is a merge
  commit (keeps one approved `[N.M.K]` commit per subtask on `main`), squash
  only when the branch carries noise. User's call per PR.
- **Flaky checks:** a transient red in an already-closed test came from other
  Claude sessions opened in codexion (their stop hook runs `make check` on the
  same `build/`). Re-run `make check` to confirm; a `flock` in the harness is a
  possible later fix (needs the user's exception).
- **Parked work:** none (the `wip-3.4.4` stash was dropped).
- **Closed in task 3.4:** 3.4.1 (`7635017`), 3.4.2 (`8977acd`), 3.4.3 (`4a60ab4`).
- **Previous:** task 3.3 done and merged (PR #11, merge commit `f065027`).
  Tasks 3.1 and 3.2 merged (PRs #9, #10). Phase 03 is `PHASE_APPROVED`.
- **Start:** 2026-10-28
- **Engine/role:** leader → phase gate (Codex) → user OK → per-subtask cycle

- **5.2.4 gates:** plan `PLAN_APPROVED` (`progress/plan_review_5.2.4.md`);
  code `APPROVED` (`progress/review_5.2.4.md`); close documentation, PR
  narrative, and tracking complete. Supplied outside-sandbox `make check`
  exited 0.

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

Proceed to subtask 5.3.3 (Free coders): review its approved spec and run the
plan gate on `feat/5.3-coders`.

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
3.4.5) are explained by concurrent `make` runs: the stop hook runs `make check`
at the end of each leader turn while a background implementer is building, and
`test_1.1.1.c` runs `make -C coders fclean`. None reproduced in sequential runs
(200 direct runs of 3.1.2, repeated full `make check`). Always re-run `make check`
sequentially before trusting a red result.

Note: avoid running two `make` processes at once; acceptance tests call
`make -C coders` themselves and a concurrent build can make them fail
spuriously (seen once during 3.3.1).

Note: agents cannot delete files under `coders/` (the guard blocks shell `rm`
and there is no delete tool). Deletions declared by a spec and stale build
objects of moved sources are done by the user by hand.
