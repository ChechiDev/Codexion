# Phase 08 tasks — Codex advisory review (2026-11-05)

Advisory only (no gate, no seal). Applied in progress/phase08_tasks_proposal.md (v2).

1. Split OK (3 subtasks, linear depends for shared main.h/Makefile). Thread start/join and zero-compiles before spawn belong to phase 09.
2. 8.1.1: real race: read last_compile_start AND the clock inside the same coder mutex (as set_compile_start does); a valid observed burnout is terminal; validate coder, coder->sim, out; publish *burned only after a successful unlock; do not exclude coders that met their own target.
3. 8.1.2: one-at-a-time reads are sound (counters only grow); validate sim, coders array, n > 0, out; *met untouched on any error incl. unlock; 0 required → met at once; completed compiles only.
4. 8.2.1: 0 required wins (stop without burnout even if a deadline passed); burnout before target for positive targets (observation policy); never set_stopped before print_burnout (its -1 also means "already stopped"); on log failure still try stop + wake, no infinite retries; never wake holding coder/log/stop mutex; define NULL arg, start_ms set before start; is_stopped hides lock failures as 1; 1 coder needs no special case; print immediately on detection (do not finish the scan); 1 ms sleep alone does not guarantee 10 ms.
5. Q1: use >= (> grants an extra ms with the integer clock).
6. Tests: controlled clock at deadline-1/deadline/deadline+1, concurrent renewal via barriers (no probabilistic sleeps), input/lock/clock faults with outputs untouched; 8.1.2 one/many coders, concurrent increments; 8.2.1 real monitor on an initialised sim: non-first coder burns, burnout during compile, global target, satisfied coder burns, 0 required with expired deadline, prior/concurrent stop, several expired → exactly one terminal line; real waiters woken, bounded joins; concurrent pipe reader measuring reception ≤ 10 ms vs known deadline (declared CI slack); TSan explicitly.
