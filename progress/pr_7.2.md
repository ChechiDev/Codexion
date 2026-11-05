## Overview

Task 7.2 adds the coder orchestration boundary between dongle arbitration and the coder thread routine. It acquires either the coder's pair or the single dongle in a one-coder simulation, then reports each acquired resource through the required log line. The caller sees ownership only after all corresponding log writes succeed.

## Approach

The implementation composes the existing `take_dongles` and `take_single_dongle` operations with `print_state`, `release_dongles`, and `is_stopped`. A local acquisition result protects the caller's output on failure. Pair and single-resource paths share the same result policy, while the one-coder path avoids locking or logging the same dongle twice. A failed acquisition is left to the acquisition API's failure contract; a failed log triggers one release attempt, after which a successfully observed stop takes precedence over the logger's ambiguous `-1` result.

## Decisions

The approved 7.2.1 spec settled on local ownership state, no retry after an acquisition or release error, and stop precedence only after a successful release. The wrapper does not set stop or wake waiters; that exit policy belongs to its caller in task 7.3.

## Issues resolved

**Acquisition failure ownership.** During advisory review, the underlying pair and single acquisition routines were found to have failure paths where resource state could already have changed, so promising that every `-1` leaves all dongles untouched would have been inaccurate. The alternative of retrying or cleaning up in the wrapper was rejected because it could double-release or conflict with failure handling owned by the lower-level APIs. The spec was clarified to preserve the caller's output while returning directly on acquisition failure and deferring resource ownership details to those APIs; the approved implementation follows that contract.

**Log failure versus stop.** The logger returns `-1` both when it drops a line because stopping began and when output fails. Returning success for every such error would hide a real output failure, while always returning failure would misclassify a stop observed immediately afterward. The contract now releases once, treats a successful release followed by an observed stop as normal stop, and otherwise returns `-1`; a release failure is not retried. This was clarified during advisory review and covered by the spec's AC4 and code-review check.

## Issues open

Nothing open for task 7.2. Phase 09 retains lifecycle error reporting as an explicit later concern for thread routines that return `NULL` on both normal and error exits.

## Follow-ups

Task 7.3 consumes this operation in `run_coder` and owns the failure exit policy: release held resources once, request stop, and wake waiters. The separate monitor and lifecycle remain in later phases, as recorded in the phase 07 review.

## Branch notes


## Where to start reviewing

1. `docs/plan/phase-07-coder-thread/7.2.1-take-coder-dongles.md` — the sealed contract and acceptance criteria.
2. `coders/src/coder/take_coder_dongles.c` — the pair/single composition and failure policy.
3. `coders/src/dongle/take_dongles.c` and `coders/src/dongle/take_single_dongle.c` — the lower-level ownership behavior that shapes the wrapper's contract.
4. `tests/acceptance/test_7.2.1.c` — observable public behavior across the six criteria.
