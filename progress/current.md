# Current session

- **State (2026-11-05):** task 7.3 Coder routine is complete on `feat/7.3-coder-routine`; subtask 7.3.1 has plan `PLAN_APPROVED`, code `APPROVED`, and the supplied outside-sandbox close check exited 0. Reference docs, plan status, task PR narrative, and commit message are prepared for the harness close.

- **State (2026-11-04, end of session):** **Phase 06 Dongle arbitration is done (9/9).** Task 6.3 Acquisition and release closed (6.3.1 `0be9980`, 6.3.2 `7800023`, 6.3.3 `26fd3e2`, 6.3.4 `6bc2d1d`, 6.3.5 `1d0de26`); PR #26 merged into `main` (merge commit `cc3dd5a`), branch deleted locally and on origin. On `main`, tree clean. PR #26's "Issues" section was expanded by Codex to 13 resolved problems (documenter rule: every problem met), applied with `gh pr edit`; the local `progress/pr_6.3.md` edit was discarded.
- **Pending (in order):** (1) After the task 7.3 PR is merged and the user approves the phase transition, break down phase 08 Monitor thread. (2) Global-harness ideas, user's call only (`.claude/agents` and `harness/` are symlinks to `~/.claude-codex-harness`, never edit them): implementer warnings, sealed-oracle fix procedure in leader.md (note in current.md + re-run plan gate), a mode to regenerate an open PR's body, `flock` around `make check`, anti-attribution hook false positive on paths containing "claude".

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
- **Phase 06 breakdown (2026-11-03):** tasks 6.1 Requests, 6.2 Readiness,
  6.3 Acquisition and release approved by the user after a Codex advisory
  (`progress/phase06_tasks_proposal.md`, `..._advisory.md`); 9 specs
  6.1.1-6.3.5 validated one by one (6.3.3 release before 6.3.4 take; 6.3.5
  had its own advisory, `progress/advisory_6.3.5_spec.md`). Verb `wake_`
  added by PR #23 (merged). Phase gate `PHASE_APPROVED` on round 3
  (`progress/phase_review_06.md`; rounds: linear depends chain for the shared
  main.h/Makefile, 6.2.2 `LONG_MAX` is representable, 6.3.2 cond-choice AC
  moved to code review, 6.3.3 reads the release time after locking + a
  contention AC). User OK given; tasks 6.1 (6.1.1 and 6.1.2) and 6.2 (6.2.1 and 6.2.2) are closed.
- **Task 5.4 complete (2026-11-03):** 5.4.1 Free sim and 5.4.2 Init simulation are closed on `feat/5.4-simulation`. The public initializer is `init_simulation`; phase review and plan review re-sealed this name after resolving conflicts with seven sealed phase 03 acceptance helpers. Subtask close documentation and task PR narrative are ready; the harness will perform the final close check and commit. See `progress/review_5.4.2.md`, `docs/reference/5.4.2-init-sim.md`, and `progress/pr_5.4.md`.
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

- **6.3.5 gates:** plan `PLAN_APPROVED` (`progress/review_history/6.3.5.md`);
  code `APPROVED` (`progress/review_6.3.5.md`); close documentation and task
  PR narrative are prepared. The supplied outside-sandbox close check exited 0.

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

`~/.claude-codex-harness` commit `8b86788` (2026-11-03, user-granted exception
for `codex/documenter.md` only, committed by the user because the project guard
blocks commit commands whose paths contain the agent names): the PR "Issues
resolved" now lists every problem met during the task (one paragraph each:
what broke, why it was missed, options, resolution) and also reads
`progress/impl_*.md`, `progress/advisory_*.md` and `progress/current.md`.
Prompted by PR #22, where the `init_sim` clash was only half a sentence (fixed
by hand in the PR body). Applies from the task 6.x PRs on.

## Next step

**Phase 07 (2026-11-05):** tasks 7.1 Coder state, 7.2 Coder acquisition, and 7.3 Coder routine are complete; all four subtasks are closed or in the current harness close. The phase gate remains `PHASE_APPROVED` (`progress/phase_review_07.md`). Key contracts include reading compile start under the coder mutex, counting only completed compiles, resolving log failure in `take_coder_dongles`, and one cleanup/stop/wake exit policy in `run_coder`; lifecycle error reporting remains for phase 09. Next: phase 08 breakdown after the task 7.3 PR is merged and the user approves the transition.

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

Note (2026-11-03): the 5.3.3 close wrote `docs/reference/5.3.3-free-coders.md`
without the `planning/execution/problems/files` section anchors; the pre-commit
`make check` caught it (nothing committed) and `doc 5.3.3` then refused
("already closed"). With the user's OK, Codex (documenter role) added only the
anchors and `./harness/review_with_codex.sh close 5.3.3` committed `83bbdb4`.
If it happens again: same fix, never hand-edit `docs/reference/`.
Same class again on 5.4.1 (2026-11-03): reference doc frontmatter had ` date:`
with a leading space ("missing frontmatter key 'date'"); Codex fixed only that
line and `close 5.4.1` committed `d1d83ab`. Codex's reference-doc formatting
slips recur: check anchors and frontmatter right after each `doc` step.

## 6.3.4 plan gate re-run: oracle data race (2026-11-04)

The sealed oracle `tests/acceptance/test_6.3.4.c` has a data race that makes
the spec's TSan gate fail (confirmed by the leader, TSan under `setarch -R`):
`run_progress_mode` (line 345) reads `owners[i]` without `owners_mutex` right
after joining only thread `i`, while threads `i+1..` still write `owners[]`
under `owners_mutex` in `run_progress` (line 304). No frame is in `coders/`.
Requested fix: assert `owners[]` only after every thread is joined (or read it
under `owners_mutex`), keeping AC6's intent; then re-seal. The implementation
(`take_dongles`) is otherwise done: `make check` 0, valgrind clean.
