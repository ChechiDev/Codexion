# Architecture (C)

> **Binding subject:** `docs/codexion-rules.md` (42 Codexion). Everything here
> applies only where it does not contradict it; on conflict the subject wins.

> What "good work" means in this repo. The hexagonal idea still applies, adapted
> to C: dependencies point toward the core; I/O and platform code sit at the edges.

## Layers (dependencies point inward)

- **core** — pure logic and data structures. No I/O, no syscalls, no globals.
  Deterministic, trivially testable. (`src/core/`, public API in `include/<name>/`.)
- **adapters** — I/O, files, sockets, OS, third-party libraries. They depend on
  core, never the other way round. (`src/io/`, `src/platform/`…)
- **interfaces** — the entry points: CLI `main`, a library's public API, a daemon.

Rule: core does not `#include` adapter headers. Dependencies are injected by
passing **opaque structs + function pointers** (the C way to define a "port").

## Modules

- One responsibility per module = one `.c` (+ private header) and, if public, one
  header under `include/<name>/`. No circular includes.
- Opaque types: expose `typedef struct foo foo_t;` in the header, keep the struct
  definition in the `.c`. Callers hold a pointer and use the module's functions.

## Memory & ownership

- Every allocation has a documented owner and a single free path.
- Prefer caller-allocates / callee-fills for hot paths; document the contract in
  the header.

## ‹Bounded contexts / modules of this project ›

- ‹module: what it owns, its public header, its dependencies›
