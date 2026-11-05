# 7.3.1 spec — Codex advisory review (2026-11-04)

Advisory only (no gate, no seal).

## Round 1: CHANGES

Applied: cleanup after a dependency unlock failure is best effort (set_stopped.c:19, wake_dongles.c:19 lock the same mutexes), accepted as unrecoverable; one release attempt per known ownership, flag cleared before the attempt, no retry; AC1 observed progress instead of 600 ms, refactor 40 ms checked, "no line after stop" dropped; AC2 compile-start spacing >= 50 ms instead of overlapping intervals, 5 ms bound on last_compile_start dropped; AC3 available == 0 under the mutex while waiting, done flag; AC4 progress observed before stop, CODEXION_TIME_SLACK_MS, interrupted compile counts 0, zero-duration case no longer claims the end-of-cycle check; AC5 watchdog; AC6 adds compile_count = INT_MAX after a take and next_ticket = LONG_MAX; AC7 adds sim NULL; single-coder release -1 → exit policy; wake_dongles always called; preconditions; review check split by topic.

## Round 2: CHANGES

Applied: AC4 split: prompt stop (stop after the first compiling line, earlier completions allowed) and new AC5 interrupted compile (compile 60000 ms, longer than the watchdog; stop after the first compiling line, counts 0, no debugging). ACs renumbered AC1-AC8.

## Round 3: READY

Round-2 point applied correctly; no remaining issue.

## Round 4 (full fresh review, user request): READY

Contract, AC1-AC8, code review check, Norm feasibility and wording: none. Note: keep AC5 compile (60 s) longer than any widened valgrind/TSan watchdog.
