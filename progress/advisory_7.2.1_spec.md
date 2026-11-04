# 7.2.1 spec — Codex advisory review (2026-11-04)

Advisory only (no gate, no seal).

## Round 1: CHANGES

Applied: acquisition -1 does not guarantee nothing held (take_dongles.c:95/115,
take_single_dongle.c:79/97), no retry; stop precedence after a log -1
documented; proposal reconciled (function releases, taken 0); AC1 coder index
2 (id 3), sentinels, other dongles unchanged; AC4 also one coder; AC5 taken
unchanged when non-NULL; new AC6 acquisition failure via next_ticket =
LONG_MAX on both branches; watchdog; local acquisition result; ticket_mutex in
preconditions; never calls set_stopped/wake_dongles; release outcomes
explicit; no direct clock/sleep.

## Round 2: CHANGES

Applied: pre-enqueue acquisition failures return directly (take_dongles.c:111, take_single_dongle.c:93), fatal path only after queuing; unlocked guarantee scoped to the wrapper (locks no mutex itself); proposal requires release 0 before stop precedence.

## Round 3: CHANGES

Applied: no claim about the dependencies internal paths (a push+unlock failure before queuing also runs the fatal path, take_single_dongle.c:39); the spec now defers to 6.3.4/6.3.5 for their failure handling.

## Round 4: READY

Round-3 point applied correctly; no remaining issue.
