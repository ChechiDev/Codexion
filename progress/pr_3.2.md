## Overview

Task 3.2 adds the first shared simulation state and its public stop-state
reader and writer. The state adapter now gives callers a mutex-protected way to
observe the stop flag and a coordinated transition that serializes with future
ordinary log output.

## Approach

The task keeps synchronization in `src/state/` and exposes the minimal `t_sim`
through the single project header. `is_stopped` reads the flag under
`stop_mutex`. `set_stopped` follows the logger's `log_mutex → stop_mutex`
ordering, changes the flag while both locks are held, and attempts both
unlock operations in reverse order. It neither prints nor wakes threads; those
responsibilities remain with later logging and lifecycle work.

## Decisions

The stop predicate returns `1` when it cannot safely read the flag, following
the approved spec's conservative caller-stop contract. The setter reports
mutex operation failures as `-1`, leaves the flag unchanged if acquisition
fails, and does not roll back a committed transition if an unlock fails.

## Issues resolved

No implementation changes were requested at the task's code gates. The 3.2.1
plan gate migrated the older sealed header oracle to allow exactly the
`pthread.h` include required for the shared mutex type and resealed that
acceptance test.

## Issues open

Nothing open.

## Follow-ups

The initial `t_sim` mutexes are initialized and destroyed by tests until the
simulation initialization work in phase 05 extends the type and owns their
lifecycle. This task does not broadcast condition variables; later resource
and lifecycle work owns thread notification and shutdown.

## Branch notes

## Where to start reviewing

1. `docs/plan/phase-03-time-and-log/3.2.1-is-stopped.md` — the state shape,
   guarded reader contract, and prior-oracle migration.
2. `coders/src/state/is_stopped.c` — the reader implementation.
3. `docs/plan/phase-03-time-and-log/3.2.2-set-stopped.md` — the writer's lock
   protocol and error postconditions.
4. `coders/src/state/set_stopped.c` — the coordinated stop transition.
