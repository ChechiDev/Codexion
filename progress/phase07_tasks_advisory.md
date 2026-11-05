# Phase 07 tasks — Codex advisory review (2026-11-04)

Advisory only (no gate, no seal). Applied in progress/phase07_tasks_proposal.md (v2).

1. Split OK; fold wait_for_burnout into a static helper of run_coder; layer of 7.1 must match the coder/ orchestration module.
2. 7.1.1: validate coder->sim, success return, untouched on clock failure, unlock before logging/sleeping; "never lags the log" too strong. 7.1.2: increment at completion; INT_MAX → -1, unchanged; interrupted compile never counts. 7.2.1: validation, *taken unchanged on failure, stop between the two logs, ownership published only after logging; release_dongles may return -1 after marking released (release_dongles.c:92), no blind retry or double release. Single-coder wait: positive slice, clock failure escapes to fatal cleanup, never prints burnout. 7.3.1: sleep_ms -1 = stop or clock failure (sleep_ms.c:31); one cleanup policy for every setter/logger/sleep/release result; zero-duration sleeps do not check stop; NULL return cannot tell failure from normal end (lifecycle error reporting, phase 09).
3. Count at completion (codexion-rules.md:137); on any -1 exit request stop + wake unconditionally; fold the wait; tests without monitor/main are enough for phase 07.
4. Tests: init_simulation, start_ms before spawn, drain captured output concurrently, set_stopped + wake_dongles, join, free; synchronized observations, hang guards; fault branches by source review.
