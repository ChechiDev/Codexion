## Overview

Task 5.4 completes construction and teardown of the full simulation state. `free_sim` releases coder and dongle arrays and the simulation coordination mutexes; `init_simulation` builds the corresponding state from a validated configuration and uses that teardown path to roll back partial construction. Together these operations provide the lifecycle boundary needed before later phases start worker threads.

## Approach

The lifecycle is layered from individual resources to arrays and then to the whole simulation. Per-dongle and per-coder operations own their synchronization objects; array initializers build elements in order and release only the initialized prefix on failure. The simulation initializer creates its coordinating mutexes, then the dongle and coder arrays. Full cleanup reverses those dependencies. Configuration is copied, the simulation start time remains zero until thread startup, and the coder back pointers refer to the enclosing simulation.

## Decisions

The public constructor is named `init_simulation`. Phase review found that `init_sim` would conflict with static helpers in seven already sealed phase 03 acceptance files. The phase breakdown and subtask oracle were re-gated with the longer name. The single-coder mapping keeps both dongle indices at zero, matching the one-dongle ring.

## Issues resolved

Phase review initially moved build and Norm checks out of behavioral acceptance criteria and into code review checks, then clarified dependencies so shared header and build-file changes follow the required construction order. Later review found the `init_sim` name collision and a stale source/oracle naming mismatch; the implementation and oracle were aligned to `init_simulation`, and the phase and plan gates were renewed. Subtask review histories for the task record no other blocking findings.

## Issues open

The initializer's pthread and allocation failure branches are reviewed structurally because those failures are not portably forceable through the public acceptance API. No blocking issue remains in the task review records.

## Follow-ups

The simulation's `start_ms` is deliberately zero here; the thread-start phase will set the actual clock origin. Callers must join worker threads before releasing a live simulation with `free_sim`.

## Branch notes


## Where to start reviewing

1. `coders/src/init/init_simulation.c` — follows the full construction sequence and its rollback paths.
2. `coders/src/cleanup/free_sim.c` — shows the reverse whole-simulation teardown used by initialization failures.
3. `docs/plan/phase-05-simulation-state/5.4.2-init-sim.md` — states the constructor contract, dependencies, and acceptance criteria.
4. `docs/architecture.md` — places initialization and cleanup responsibilities in the adapter map.
