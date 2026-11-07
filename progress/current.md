# Current session

- **ISS-32.1 (2026-11-06):** close prepared on `refactor/ISS-32-refactor-sleep-ms-returns-1-on-stop-1-only-on-error-3-3`; code review is `APPROVED` (`progress/review_ISS-32.1.md`) and supplied outside-sandbox close `make check` exited 0. Reference note, issue status, generated index, history, and task ISS-32 PR narrative are updated. Commit message is in `progress/close_ISS-32.1.msg`; harness will run the final check and commit. Next: phase 09 planning after the harness opens the ISS-32 PR and the user approves the transition.

- **State (2026-11-05, handoff to a new session):** **Phase 08 Monitor thread complete** (8.1.1 `a87183d`, 8.1.2 `3445344`, 8.2.1 `18b76df`). PR #31 (task 8.2) merged into `main` (merge commit `225298b`), branch deleted locally and on origin. On `main`, clean. `make status` → `Next: break down phase 09 into tasks and subtask specs`. Phase 09 (`docs/plan/phase-09-lifecycle.md`) is not broken down yet: plan it subtask by subtask with the user (one question per message, validate each spec before writing the next), then the `phase 09` gate, then stop for the user's OK. Before any gate, check that no other session is changing `~/.claude-codex-harness` (see the 8.1.2 code gate note at the end).
- **8.2.1 (2026-11-05):** close prepared on `feat/8.2-monitor-routine`; code review is `APPROVED` (`progress/review_8.2.1.md`) and supplied outside-sandbox close `make check` exited 0. Reference documentation, plan status, generated index, history, and task 8.2 PR narrative are updated. Commit message is in `progress/close_8.2.1.msg`; harness will run the final check and commit.
- **Task 8.2 complete:** monitor checks and monitor routine are documented and marked done. Task PR narrative is `progress/pr_8.2.md`.

- **State (2026-11-05, end of session):** **Task 8.1 Monitor checks done** (8.1.1 `a87183d`, 8.1.2 `3445344`); PR #30 merged into `main` (merge commit `668ff85`), branch deleted locally and on origin. On `main`. Next: task 8.2, subtask 8.2.1 `run_monitor` (create the task branch from `main`, in_progress, plan gate). Before any gate, check that no other session is changing `~/.claude-codex-harness` (see the 8.1.2 code gate note at the end). Remind the implementer: inner TDD test-first; keep the tie-break in one static helper (Defense note in the 8.2.1 spec).
- **8.1.2 (2026-11-05):** close prepared on `feat/8.1-monitor-checks`; code review is `APPROVED` (`progress/review_8.1.2.md`) and supplied outside-sandbox close `make check` exited 0. Reference note, plan status, generated index, history, and task 8.1 PR narrative are updated. Commit message is in `progress/close_8.1.2.msg`; harness will run the final check and commit.
- **Task 8.1 complete:** 8.1.1 `get_burned_out` and 8.1.2 `get_target_met` are documented and marked done. Task PR narrative is `progress/pr_8.1.md`.
- **Next:** task 8.2 monitor routine, after the harness completes the 8.1 close and the user approves the transition.

Note (2026-11-05, 8.1.2 code gate): the first `code 8.1.2` round returned
`CHANGES_REQUESTED` only because the already-closed `tests/acceptance/test_ISS-14.1.c`
AC5 (`test_ac5_binary_output`, runs `coders/codexion`) failed inside the gate's
`make check`. Three sequential `make check` runs right after were green and the
re-run gate was `APPROVED`. Probable cause (not proven): a parallel session was
changing the global harness at the same time (`~/.claude-codex-harness` PR #15
merged 16:13, `lib/` files rewritten 16:17:59, including `check.sh` and
`_guard.py`; `harness/` here is a symlink to that `lib/`), so the gate may have
mixed script versions or overlapped another `make check` on the same `build/`.
Procedure: if a gate fails on a test unrelated to the subtask, re-run
`make check` sequentially before touching anything; avoid running gates while
another session merges, installs or runs checks on the global harness.

- **ISS-34.1 BLOCKER (2026-11-06, implementer):** `print_state.c` and `tests/unit/test_print_state.c` are done and the unit test plus test_3.4.{3,4,5,6} pass, but sealed `tests/acceptance/test_ISS-34.1.c` AC6(b) aborts at line 449 (`sim.dongles[first].heap.size == 0`). Cause is unrelated to `print_state`: on a simulation already stopped, `take_dongles` (`coders/src/dongle/take_dongles.c`, `push_requests` then `take_pair` returns 0 on `is_stopped` without popping) leaves its request in both dongle heaps (size 1), while the oracle expects empty heaps. `take_coder_dongles` returns 0 with `taken == 0` and nothing is printed, as expected. Fixing it needs `take_dongles.c` (outside the declared `files:`), or the oracle/spec AC6(b) must drop the heap-empty assertion (re-run plan gate per "fix sealed oracle via plan gate"). Awaiting leader decision.
- **ISS-34.1 oracle fix (2026-11-06, leader, user OK):** the "empty heaps" condition in AC6(b) was a leader spec error (it contradicts the sealed 6.3.4 stop contract, which does not pop the queued request). Removed from the spec; plan gate re-run so Codex drops the two `heap.size == 0` assertions from `tests/acceptance/test_ISS-34.1.c` and re-seals. No production change outside `print_state.c`.

- **ISS-34.1 (2026-11-06):** close prepared on `refactor/ISS-34-refactor-print-state-returns-1-on-stop-1-only-on-error-3-4`; code review is `APPROVED` (`progress/review_ISS-34.1.md`) and supplied outside-sandbox close `make check` exited 0. Reference note, issue status, generated index, history, and task ISS-34 PR narrative are updated. Commit message is in `progress/close_ISS-34.1.msg`; harness will run the final check and commit. Task ISS-34 is ready for the harness PR step.
