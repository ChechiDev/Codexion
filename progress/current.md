# Current session

- **Current subtask:** 3.4.2 Build log line (task 3.4 "Serialized logger",
  last task of phase 03), closed on branch `feat/3.4-serialized-logger`.
  Plan gate: `PLAN_APPROVED`; code gate: `APPROVED`; close verification supplied
  by the harness (`make check` exit code 0). Subtasks 3.4.1 and 3.4.2 are closed.
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

Start subtask 3.4.3 (leader: mark `in_progress` and run `make plan`), then
complete 3.4.3 and 3.4.4. Closing 3.4.4 opens the task 3.4 PR and completes
phase 03. Every subtask that edits `coders/Makefile` or `coders/main.h` must
refresh the 42 header `Updated:` lines with Stdheader.

Note: avoid running two `make` processes at once; acceptance tests call
`make -C coders` themselves and a concurrent build can make them fail
spuriously (seen once during 3.3.1).

Note: agents cannot delete files under `coders/` (the guard blocks shell `rm`
and there is no delete tool). Deletions declared by a spec and stale build
objects of moved sources are done by the user by hand.
