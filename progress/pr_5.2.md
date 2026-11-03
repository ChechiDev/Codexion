## Overview

Task 5.2 provides the dongle lifecycle needed by the simulation state: initialize one dongle, release its resources, tear down an initialized array, and construct an independently initialized array. Each dongle owns its mutex, condition variable, and two-slot request heap. The array initializer uses the established single-dongle lifecycle APIs so a partial build can be rolled back through the same cleanup path used at shutdown.

## Approach

The task separates per-element lifecycle from array orchestration. `init_dongle` establishes one usable dongle and rolls back its own partial setup. `destroy_dongle` releases those resources in reverse order while attempting all teardown steps. `free_dongles` destroys a specified initialized prefix in reverse order and frees the array. `init_dongles` allocates the array, initializes elements in index order, and delegates prefix rollback to `free_dongles` if an element fails. The public output pointer is written only after complete success.

## Decisions

The approved plan keeps pthread and allocation lifecycle in the `init/` and `cleanup/` adapters, while request storage operations remain in the core heap API. Teardown accepts the number of successfully initialized elements so the array cleanup function can serve both normal shutdown and initialization rollback. Allocation and pthread failure paths that cannot be induced portably are explicit code-review checks.

## Issues resolved

The Phase 05 review requested removal of symbol-table and build/Norm checks from behavioral acceptance criteria, moving build and style checks to the code review gate. It also required dependencies to sequence shared `main.h` and Makefile edits; the approved graph makes 5.2.1 depend on seat mapping and later coder initialization depend on the completed dongle array branch. The 5.2 subtask plan and code gates approved the resulting lifecycle contracts without blocking findings.

## Issues open

Nothing open.

## Follow-ups

Task 5.3 uses the completed dongle initializer and `get_dongle_pair` to initialize coders. Task 5.4 will compose dongles and coders into whole-simulation initialization and teardown, including reverse rollback. Callers must stop and join any users before destroying dongles, as thread lifecycle is handled later.

## Branch notes

The branch contains only task 5.2 work after its task base: commits for subtasks 5.2.1, 5.2.2, and 5.2.3, followed by this final close. No unrelated branch changes are recorded in the task sources reviewed here.

## Where to start reviewing

1. `docs/plan/phase-05-simulation-state/5.2.4-init-dongles.md` — the approved array contract and rollback rules.
2. `coders/src/init/init_dongles.c` — allocation sizing, ordered initialization, and prefix rollback.
3. `coders/src/cleanup/free_dongles.c` — the shared teardown path for rollback and normal array release.
4. `tests/acceptance/test_5.2.4.c` — public API cases for array state and independence.
