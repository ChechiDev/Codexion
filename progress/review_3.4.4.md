# Code review — subtask 3.4.4

**Verdict:** CHANGES_REQUESTED

## Checkpoints
- C0: [x] The burnout text, allowed calls, lock order, and terminal stop behavior match the subject and spec; the supplied harness reports Norm green.
- C1: [x] The supplied `make check` exited 0 and reports the required harness files and invariants present.
- C2: [x] One subtask is in progress, the generated plan is synchronized, and its plan gate is approved.
- C3: [x] The logger stays in the adapter layer, uses `t_sim`, and adds one public function.
- C4: [ ] `coders/src/log/print_burnout.c:28-43` duplicates the log-line output path in `coders/src/log/print_state.c:37-52`. The new static names at `print_burnout.c:17,28` also violate the closed verb list in `docs/conventions.md`.
- C5: [x] The sealed acceptance oracle has one test per AC1–AC12; the supplied green harness log reports tests passed. The unit test covers success, rejection, lock failures, and write failure. No sanitizer result was supplied.
- C6: [ ] Close work (reference document, done status, history, commit) is pending this code gate.

## Findings
1. `coders/src/log/print_burnout.c:28-43` repeats the timestamp, `build_log_line`, and single `write` sequence already implemented at `coders/src/log/print_state.c:37-52`. The reviewer contract requires rejecting duplicated logic that should be reused. A single shared output path should serve both loggers while preserving their different stop protocols and the specified mutex coverage.
2. `coders/src/log/print_burnout.c:17,28` introduces `claim_stop` and `write_line`. `docs/conventions.md` requires every function, including static helpers, to start with a verb from its closed list; neither `claim_` nor `write_` is listed.

## Requested changes
1. Rework the plan so both loggers can use one output helper, then pass the required plan gate before changing any file outside the current declared scope. Preserve one unbuffered write and all error and lock behavior in the spec.
2. Rename new static helpers to comply with the closed verb list, or obtain an approved style change before using new verbs.
