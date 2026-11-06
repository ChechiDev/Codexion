## Overview

ISS-34 gives `print_state` a distinct result for a normal stopped simulation. A written line returns `0`, a suppressed line after stop returns `1`, and invalid input or an operational failure returns `-1`. This lets later runtime handling distinguish shutdown from failure while preserving the current callers' behavior.

## Approach

The change stays in the logging adapter. It validates the state and coder before locking, then keeps the existing `log_mutex` → `stop_mutex` order for the stop check and serialized output. The stop mutex is released before timestamp lookup and the one-write line output. Existing `run_coder` and `take_coder_dongles` paths continue to leave on any nonzero result. The acceptance coverage exercises the contract directly, checks callers through their public APIs, and migrates only the sealed assertions that treated stop as `-1`.

## Decisions

The three-way result matches the `sleep_ms` contract from ISS-32, so stopped outcomes and real failures remain distinguishable at the API boundary. The user selected this contract over rechecking stop state in `run_coder`, which could hide a write failure that coincides with stop. The issue is delivered as a separate issue task because it updates sealed acceptance oracles. `print_burnout` and caller implementations remain unchanged.

## Issues resolved

**Invalid coder input after stop.** The first issue review found that changing the stopped branch alone could return `1` for an invalid coder, since coder validation originally occurred later in the formatting path. The option of accepting that behavior would violate the stated failure contract; moving validation before the stop read keeps the declared scope to `print_state.c`. The issue review requested and approved the revised contract, and AC5 covers invalid coder and state inputs after stop.

**Stopped caller heap expectation.** During implementation, AC6(b) asserted that `take_coder_dongles` left empty request heaps after a stopped call. That was not caught earlier because it conflicted with the already sealed 6.3.4 behavior, which returns on stop without popping queued requests. Changing acquisition code would have expanded this task beyond its declared file and altered that contract. The issue spec and oracle were corrected through the issue and plan review gates; AC6(b) now checks the return value, silence, and dongle availability, while teardown frees the heaps.

## Issues open

Nothing open.

## Follow-ups

Phase 09 runtime failure handling can use `print_state`'s `-1` result to distinguish real errors from normal stop results. The current callers still treat all nonzero values alike, as specified for this task.

## Branch notes


## Where to start reviewing

1. `coders/src/log/print_state.c` — validation order, stop result, and error precedence.
2. `docs/plan/issues/ISS-34-refactor-print-state-returns-1-on-stop-1-only-on-error-3-4/ISS-34.1-print-stop-result.md` — sealed contract and acceptance scope.
3. `tests/acceptance/test_ISS-34.1.c` — public behavior and caller regression criteria.
4. `tests/acceptance/test_3.4.3.c` — representative migration of the prior stop expectation.
