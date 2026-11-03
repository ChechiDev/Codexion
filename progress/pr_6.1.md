## Overview

Task 6.1 provides the request identity and policy key needed by dongle arbitration. `take_ticket` allocates one synchronized simulation-wide arrival number, and `build_request` combines that ticket with the coder id and the configured FIFO or EDF key. The resulting request is ready for later arbitration code to place consistently in the coder's dongle heaps.

## Approach

The ticket counter stays in the simulation state adapter and is protected by its own mutex. Request construction reads `last_compile_start` under the coder mutex, releases that lock, obtains a ticket, and delegates key calculation to the pure policy API. The builder returns the completed value to its caller; queue mutation remains the responsibility of the later acquisition subtasks. This keeps state synchronization, policy calculation, and orchestration responsibilities separate.

## Decisions

No user-directed or reviewer-directed decisions changed the approved task scope. A ticket may be consumed if EDF key calculation subsequently fails; ticket order must be strictly increasing, so preserving contiguity is unnecessary.

## Issues resolved

**Request builder test fixture lifecycle.** Repeated setup required destroying the fixture's coder and ticket mutexes before reinitialization and at exit; otherwise the test could reinitialize live pthread mutexes. This surfaced during implementation testing, after the acceptance plan had been reviewed, because the acceptance contract did not cover unit-fixture reuse. The options were to avoid reinitializing the fixture or to give the fixture an explicit teardown; teardown was chosen so tests could keep isolated setup while respecting pthread lifecycle. The correction is in `tests/unit/test_build_request.c` and was confirmed during code review.

**Helper naming convention.** Source review caught the initial helper name `read_last_start`, whose verb is outside the project's allowed list. The choices were to rename it or expand the convention; the helper was renamed to `get_last_start` because it is a local operation and did not justify changing the shared naming rules. The fix is in `coders/src/dongle/build_request.c`; behavior and public contract did not change.

## Issues open

Nothing open.

## Follow-ups

The builder deliberately does not lock dongles or insert into heaps. Later arbitration subtasks must build the request before taking dongle mutexes, then preserve the same request identity across the relevant heaps. The broader phase also retains the full cooldown and stop-wakeup behavior for its acquisition and release work.

## Branch notes

## Where to start reviewing

1. `docs/plan/phase-06-dongle-arbitration/6.1.1-take-ticket.md` and `6.1.2-build-request.md` — approved contracts and acceptance criteria for the task.
2. `coders/src/state/take_ticket.c` — synchronized, bounded simulation-wide ticket allocation.
3. `coders/src/dongle/build_request.c` — composes coder state, ticket allocation, and policy key calculation.
4. `docs/architecture.md` — shows where the state and orchestration modules sit in the project.
