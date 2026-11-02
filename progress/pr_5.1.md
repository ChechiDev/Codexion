## Overview

Task 5.1 adds the seat-to-dongle mapping that the simulation uses to associate each coder with its two neighbouring dongles. It handles the circular boundary and the one-coder, one-dongle case while returning indices in the order required by later lock acquisition.

## Approach

The mapping is a pure helper in `src/init/`: it validates the caller's seat, coder count, and output pointers, computes the wrapped neighbour without overflowing at `INT_MAX`, sorts the two indices, and writes results only after validation succeeds. The helper is exposed through the shared header and included in the deliverable Makefile. Subtask 5.1.1 is the only subtask in this task.

## Decisions

The approved plan placed this pure helper in `init/` because its caller is the coder initializer, while keeping its implementation free of mutex and platform dependencies. The outputs are ordered by index rather than by left and right position so later code can acquire both dongle locks consistently. For one coder, both output indices identify the sole dongle.

## Issues resolved

The Phase 05 gate requested that symbol-table and build/Norm checks be removed from behavioral acceptance criteria and retained as code review checks. It also requested explicit dependencies to sequence edits to the shared header and Makefile. The phase plan was revised, and the fourth phase review approved the resulting behavioral criteria and dependency graph. The 5.1.1 plan and code gates approved the function without blocking findings.

## Issues open

Nothing open.

## Follow-ups

Subtask 5.2.1 depends on 5.1.1, and 5.3.1 later consumes `get_dongle_pair` when initializing each coder. The phase plan also requires the one-coder case to be acquired only once by arbitration code.

## Branch notes

The branch starts at `f43299e`, the task 4.3 merge already on `main`. No task 5.1 commit is present yet; this close supplies the documentation and harness commit message for the current task branch.

## Where to start reviewing

1. `docs/plan/phase-05-simulation-state/5.1.1-get-dongle-pair.md` — approved contract, edge cases, and declared scope.
2. `coders/src/init/get_dongle_pair.c` — validation, wraparound, and ordered output implementation.
3. `tests/acceptance/test_5.1.1.c` — public API cases corresponding to the acceptance criteria.
