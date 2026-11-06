---
id: ISS-34.1
alias:
  - Print stop result
tags:
  - codexion
  - logging
  - concurrency
date: 2026-11-06 14:00
---

<!-- section: concept -->
## Distinguishing a stopped log from a logging failure

`print_state` serializes one coder state line while coordinating with the shared
stop flag. Its result now communicates three distinct outcomes:

| Result | Meaning | Output |
|---|---|---|
| `0` | The line was written successfully | One complete line |
| `1` | The simulation was already stopped | No bytes |
| `-1` | The input or an operation failed | No line is promised; a failed write may have emitted partial bytes |

The stop result matters to callers that must treat an ordinary shutdown
separately from an error. Input validation happens before taking locks, so an
invalid state or coder remains an error even when the simulation is stopped.
The lock order remains `log_mutex` then `stop_mutex`: the log lock serializes
both the stop decision and output, while the stop lock is released before time
lookup and writing.

```text
validate state and coder
        │
lock log_mutex
        │
lock stop_mutex → read stopped → unlock stop_mutex
        ├── stopped: unlock log_mutex → 1
        └── running: timestamp → build line → write once → unlock → 0
              any failed operation --------------------------→ -1
```

A failure still wins over a stop result if it occurs in the same call, such as
a failure to release `log_mutex`. Existing callers continue to stop their
current operation on any nonzero result. A later lifecycle task can use the
difference between `1` and `-1` to mark only real errors.

<!-- section: planning -->
## How it was planned

Issue ISS-34 addresses the ambiguity between a suppressed line and a real
failure. The approved subtask declared only
`[[coders/src/log/print_state.c]]`; caller behavior was checked through their
public APIs. Acceptance criteria cover output while running, all states after
stop, concurrent stop, rejected input and operational failures, caller
regressions, and the built symbol. The four sealed 3.4.x acceptance oracles
were migrated only where they expected a stop to return `-1`.

<!-- section: execution -->
## How it was executed

The adapter validates the coder alongside the state before locking, returns
`1` when its synchronized stop read finds the simulation stopped, and retains
`-1` for validation, lock, clock, formatting, and write failures. It keeps the
existing lock order and single-write output contract. `run_coder` and
`take_coder_dongles` remain unchanged because they already treat nonzero as a
request to leave their current operation; interpreting `1` as normal shutdown
is follow-up work for phase 09.

<!-- section: problems -->
## Problems & resolution

The first plan review found that a stopped simulation could mask an invalid
coder as a stop result. The issue spec and acceptance checks were revised to
require invalid coder IDs to remain `-1`, and the implementation validates the
coder before reading the stop flag. A later AC6(b) oracle expected queued
requests to be removed from dongle heaps after stop, contrary to the sealed
6.3.4 contract. The spec and oracle were corrected to check the caller result,
no output, and dongle availability without asserting heap cleanup; queued
requests are freed with the heaps at teardown.

<!-- section: files -->
## Files created / modified

- [[coders/src/log/print_state.c]] — distinguishes a normal stopped result from failures and validates inputs before locking.
- [[tests/acceptance/test_ISS-34.1.c]] — covers AC1–AC7, including the corrected stopped-caller expectation.
- [[tests/acceptance/test_3.4.3.c]] — migrates stop-result expectations.
- [[tests/acceptance/test_3.4.4.c]] — migrates stop-result expectations.
- [[tests/acceptance/test_3.4.5.c]] — migrates stop-result expectations.
- [[tests/acceptance/test_3.4.6.c]] — migrates stop-result expectations.
- [[tests/unit/test_print_state.c]] — expects `1` for a stopped simulation.
- [[docs/plan/issues/ISS-34-refactor-print-state-returns-1-on-stop-1-only-on-error-3-4/ISS-34.1-print-stop-result.md]] — marks the approved subtask done.
- [[docs/plan/PLAN.md]] and [[feature_list.json]] — regenerated issue status and index.
- [[progress/current.md]] and [[progress/history.md]] — record close state and history.
- [[progress/pr_ISS-34.md]] — summarizes the task for its pull request.
- [[progress/close_ISS-34.1.msg]] — supplies the close commit message to the harness.
