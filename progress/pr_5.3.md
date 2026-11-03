## Overview

Task 5.3 completes the coder-side state lifecycle needed by the simulation. It defines the coder record, initializes and destroys each coder's mutex, releases full or partially initialized coder arrays, and constructs arrays with rollback when an element cannot be initialized. Together, these operations give later simulation code a single ownership path for per-coder state.

## Approach

The task separates element lifecycle from array lifecycle. `init_coder` maps one seat and creates its mutex; `destroy_coder` releases that mutex; `free_coders` destroys an initialized prefix in reverse order and frees its allocation. `init_coders` performs overflow-safe sizing, initializes seats in order, and delegates partial rollback to `free_coders`. The simulation back pointer is borrowed, and thread creation and joining remain outside this task.

## Decisions

The approved plan keeps each public operation in its own source file and sequences edits to `coders/main.h` and `coders/Makefile` through subtask dependencies. Array initialization publishes its output only after all elements succeed, and rollback uses the already established prefix cleanup API. No implementation decisions depart from the approved specifications.

## Issues resolved

The phase review removed `nm`, build, and Norm checks from behavioral acceptance criteria, retaining build and Norm under code review. It also required dependencies that order shared header and Makefile changes and ensure the coder type follows the dongle type. The task's plan and code gates were approved without further requested changes. No implementation problems were recorded in the four task reference notes.

## Issues open

Nothing open.

## Follow-ups

Later simulation work must join any threads before destroying coder mutexes or freeing their array. The coder thread handle is reserved for that lifecycle work; this task does not start threads or access the borrowed simulation state.

## Branch notes


## Where to start reviewing

1. `coders/src/init/init_coders.c` — array allocation, ordered setup, and partial rollback.
2. `coders/src/init/init_coder.c` — per-seat mapping and mutex ownership established for each element.
3. `coders/src/cleanup/free_coders.c` — reverse teardown contract reused by rollback and final cleanup.
4. `coders/main.h` — the shared coder type and lifecycle API declarations.
