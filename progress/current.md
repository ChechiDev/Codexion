# Current session

- **Current subtask:** 3.2.1 Is stopped (task 3.2 "Stop state"), closed;
  documentation and close message prepared on branch `feat/3.2-stop-state`.
  Plan gate `PLAN_APPROVED`; code gate `APPROVED`. The supplied pre-session
  `make check` exited 0.
- **Previous:** task 3.1 done and merged (PR #9, merge commit `ab2f615`).
  Phase 02 done and merged (PR #7, merge commit `d76d218`).
  Phase 03 broken down (4 tasks, 9 subtasks, each spec validated by the user
  after a read-only Codex advisory review) and `PHASE_APPROVED`.
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

1. Harness close check and commit for 3.2.1 using `progress/close_3.2.1.msg`.
2. Then begin subtask 3.2.2, Set stopped.

Note: agents cannot delete files under `coders/` (the guard blocks shell `rm`
and there is no delete tool). Deletions declared by a spec and stale build
objects of moved sources are done by the user by hand.
