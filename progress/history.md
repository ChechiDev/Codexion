# Historical log (append-only)

> When each subtask closes, Codex appends its summary here as part of the close.
> Do not edit previous entries. Only append at the end.

<!--
Entry template (filled in at close):

## YYYY-MM-DD — Subtask <N.M.K>: <name>
- **Layer:** domain | application | infrastructure | interfaces
- **Plan:** docs/plan/phase-NN-<slug>/<N.M.K>-<slug>.md (plan_review: PLAN_APPROVED)
- **Changes:** <src/ and tests/unit files touched>
- **Acceptance:** tests/acceptance/test_<N.M.K>.py — N cases, green
- **Verification:** make check green. Codex review: APPROVED (review_<N.M.K>.md)
- **Documentation:** docs/reference/<N.M.K>-<name>.md
- **Commit:** <type>(<scope>): <what> [N.M.K]   (subject only: the hash is in git log)
-->

## 2026-10-26 — Subtask 1.1.1: Deliverable Makefile
- **Layer:** interfaces
- **Plan:** docs/plan/phase-01-build-skeleton/1.1.1-deliverable-makefile.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/Makefile`, `coders/main.c`, and `README.md` deliver the initial build skeleton.
- **Acceptance:** `tests/acceptance/test_1.1.1.c` — AC1–AC9 covered and green in the supplied check.
- **Verification:** supplied `make check` green (exit code 0). Code review: APPROVED (`progress/review_1.1.1.md`).
- **Documentation:** docs/reference/1.1.1-deliverable-makefile.md
- **Commit:** feat(build): add deliverable Makefile [1.1.1]

## 2026-10-26 — Subtask 1.2.1: Configuration types
- **Layer:** core
- **Plan:** docs/plan/phase-01-build-skeleton/1.2.1-config-types.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/main.h` defines configuration and scheduling types; `coders/Makefile` tracks the shared header dependency.
- **Acceptance:** `tests/acceptance/test_1.2.1.c` — AC1–AC5 covered and green in the supplied check.
- **Verification:** supplied `make check` green (exit code 0). Code review: APPROVED (`progress/review_1.2.1.md`).
- **Documentation:** docs/reference/1.2.1-config-types.md
- **Commit:** feat(core): add configuration types [1.2.1]

## 2026-10-26 — Subtask 1.2.2: Rename scheduling policy to mode
- **Layer:** core
- **Plan:** docs/plan/phase-01-build-skeleton/1.2.2-rename-mode.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/main.h` renames the scheduling enum and configuration field; both acceptance oracles use the renamed API.
- **Acceptance:** `tests/acceptance/test_1.2.2.c` — AC1–AC4 covered and green in the supplied check.
- **Verification:** supplied `make check` green (exit code 0). Code review: APPROVED (`progress/review_1.2.2.md`).
- **Documentation:** docs/reference/1.2.2-rename-mode.md
- **Commit:** feat(core): rename scheduling policy to mode [1.2.2]

## 2026-10-26 — Subtask 1.2.3: Argument count and usage
- **Layer:** interfaces
- **Plan:** docs/plan/phase-01-build-skeleton/1.2.3-argument-count-usage.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/main.c`, `coders/main.h`, and `README.md` implement and explain the early argument-count check.
- **Acceptance:** `tests/acceptance/test_1.2.3.c` — AC1–AC5 covered and green in the supplied check.
- **Verification:** supplied `make check` green (exit code 0). Code review: APPROVED (`progress/review_1.2.3.md`).
- **Documentation:** docs/reference/1.2.3-argument-count-usage.md
- **Commit:** feat(interfaces): reject wrong argument counts [1.2.3]

## 2026-10-27 — Subtask 2.1.1: Trim span
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.1.1-trim-span.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/parsing/trim_span.c`, `coders/main.h`, and `coders/Makefile` add a non-mutating whitespace span helper; unit and acceptance coverage exercise its contract.
- **Acceptance:** `tests/acceptance/test_2.1.1.c` — AC1–AC6 covered and green in the supplied check.
- **Verification:** supplied `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.1.1.md`).
- **Documentation:** docs/reference/2.1.1-trim-span.md
- **Commit:** feat(core): add whitespace span trimming [2.1.1]

## 2026-10-27 — Subtask 2.1.2: Parse number
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.1.2-parse-number.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/parsing/parse_number.c`, `coders/main.h`, and `coders/Makefile` add a shared trimmed, digits-only conversion bounded by `INT_MAX`; unit and acceptance coverage exercise valid values, invalid input, and overflow.
- **Acceptance:** `tests/acceptance/test_2.1.2.c` — AC1–AC6 covered and green in the supplied check.
- **Verification:** supplied `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.1.2.md`).
- **Documentation:** docs/reference/2.1.2-parse-number.md
- **Commit:** feat(core): parse numbers safely [2.1.2]

## 2026-10-27 — Subtask 2.2.1: Move print usage
- **Layer:** interfaces
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.1-move-print-usage.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/cli/print_usage.c`, `coders/main.c`, and `coders/Makefile` place usage output in the CLI layer while preserving the entry point behavior.
- **Acceptance:** `tests/acceptance/test_2.2.1.c` — AC1–AC3 covered and green in the supplied check.
- **Verification:** supplied `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.1.md`).
- **Documentation:** docs/reference/2.2.1-move-print-usage.md
- **Commit:** feat(interfaces): move usage output to CLI [2.2.1]

## 2026-10-27 — Subtask 2.2.2: Move trim span
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.2-move-trim-span.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/utils/trim_span.c` and `coders/Makefile` move the generic whitespace span helper into utils; the former parsing source is removed. The 2.1.1 acceptance path check was migrated and resealed.
- **Acceptance:** `tests/acceptance/test_2.2.2.c` — AC1–AC2 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.2.md`).
- **Documentation:** docs/reference/2.2.2-move-trim-span.md
- **Commit:** refactor(utils): move trim_span into utils [2.2.2]

## 2026-10-27 — Subtask 2.2.3: Header sections
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.3-header-sections.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/main.h` groups the existing declarations under the Macros, Types, Utils, Parsing and CLI section titles without changing the public API.
- **Acceptance:** `tests/acceptance/test_2.2.3.c` — AC1–AC4 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.3.md`).
- **Documentation:** docs/reference/2.2.3-header-sections.md
- **Commit:** feat(core): organize header declarations [2.2.3]

## 2026-10-27 — Subtask 2.2.4: Trim slice
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.4-trim-slice.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/utils/trim_slice.c`, `coders/main.h`, and `coders/Makefile` add a public non-owning slice view as a thin delegate to `trim_span`; unit coverage checks edge cases and source preservation.
- **Acceptance:** `tests/acceptance/test_2.2.4.c` — AC1–AC6 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.4.md`).
- **Documentation:** docs/reference/2.2.4-trim-slice.md
- **Commit:** feat(utils): add trimmed slice view [2.2.4]

## 2026-10-27 — Subtask 2.2.5: Use trim slice
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.5-use-trim-slice.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/parsing/parse_number.c` now consumes the non-owning `t_slice` returned by `trim_slice`; parsing behavior is unchanged.
- **Acceptance:** `tests/acceptance/test_2.2.5.c` — AC1–AC3 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.5.md`).
- **Documentation:** docs/reference/2.2.5-use-trim-slice.md
- **Commit:** refactor(parsing): use trim slice in number parser [2.2.5]

## 2026-10-27 — Subtask 2.2.6: Move trim logic
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.6-move-trim-logic.md (plan_review: PLAN_APPROVED)
- **Changes:** `trim_slice.c` now owns the single trimming algorithm; `trim_span.c` delegates and maps the result into the legacy span type.
- **Acceptance:** `tests/acceptance/test_2.2.6.c` — AC1–AC4 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.6.md`).
- **Documentation:** docs/reference/2.2.6-move-trim-logic.md
- **Commit:** refactor(utils): move trim algorithm to slice [2.2.6]

## 2026-10-27 — Subtask 2.2.7: Remove trim span
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.7-remove-trim-span.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/main.h` and `coders/Makefile` retire the span API and its build entry; the delegate and obsolete unit test were removed, with edge cases retained against `trim_slice`.
- **Acceptance:** `tests/acceptance/test_2.2.7.c` — AC1–AC3 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.7.md`).
- **Documentation:** docs/reference/2.2.7-remove-trim-span.md
- **Commit:** chore(utils): remove trim span API [2.2.7]

## 2026-10-27 — Subtask 2.2.8: Is space
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.8-is-space.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/utils/is_space.c`, `coders/main.h`, and `coders/Makefile` add the generic six-character whitespace predicate; unit and acceptance coverage exercise its contract.
- **Acceptance:** `tests/acceptance/test_2.2.8.c` — AC1–AC4 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.8.md`).
- **Documentation:** docs/reference/2.2.8-is-space.md
- **Commit:** feat(utils): add whitespace predicate [2.2.8]

## 2026-10-27 — Subtask 2.2.9: Use is space
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.9-use-is-space.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/utils/trim_slice.c` now uses the shared `is_space` predicate in both boundary scans, preserving the established trimming behavior.
- **Acceptance:** `tests/acceptance/test_2.2.9.c` — AC1–AC3 covered and green in the supplied check; the leading offset oracle was corrected from 6 to 7 during the plan gate.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.9.md`).
- **Documentation:** docs/reference/2.2.9-use-is-space.md
- **Commit:** refactor(utils): use is_space in trim_slice [2.2.9]

## 2026-10-27 — Subtask 2.2.10: Is digit
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.10-is-digit.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/utils/is_digit.c`, `coders/main.h`, and `coders/Makefile` add the generic decimal digit predicate; unit and acceptance coverage exercise its contract.
- **Acceptance:** `tests/acceptance/test_2.2.10.c` — AC1–AC4 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.10.md`).
- **Documentation:** docs/reference/2.2.10-is-digit.md
- **Commit:** feat(utils): add decimal digit predicate [2.2.10]

## 2026-10-27 — Subtask 2.2.11: Use is digit
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.2.11-use-is-digit.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/parsing/parse_number.c` now uses the shared `is_digit` predicate while preserving trimming, overflow checks, and output-on-success behavior.
- **Acceptance:** `tests/acceptance/test_2.2.11.c` — AC1–AC3 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.2.11.md`).
- **Documentation:** docs/reference/2.2.11-use-is-digit.md
- **Commit:** refactor(parsing): use is_digit in number parser [2.2.11]

## 2026-10-28 — Subtask 2.3.1: Get arg minimum
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.3.1-get-arg-minimum.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/parsing/get_arg_minimum.c`, `coders/main.h`, and `coders/Makefile` provide a shared positional lookup for numeric argument minima.
- **Acceptance:** `tests/acceptance/test_2.3.1.c` — AC1–AC4 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.3.1.md`).
- **Documentation:** docs/reference/2.3.1-get-arg-minimum.md
- **Commit:** feat(parsing): add argument minimum lookup [2.3.1]

## 2026-10-28 — Subtask 2.3.2: Parse mode
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.3.2-parse-mode.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/parsing/parse_mode.c`, `coders/main.h`, and `coders/Makefile` add exact `fifo`/`edf` parsing with output unchanged on failure.
- **Acceptance:** `tests/acceptance/test_2.3.2.c` — AC1–AC4 covered and green in the supplied check.
- **Verification:** supplied pre-session check reported a missing `date` frontmatter key in the reference note; the indentation was corrected for the harness rerun. Acceptance checks and build passed in the supplied log. Code review: APPROVED (`progress/review_2.3.2.md`).
- **Documentation:** docs/reference/2.3.2-parse-mode.md
- **Commit:** feat(parsing): parse scheduler mode [2.3.2]

## 2026-10-28 — Subtask 2.3.3: Parse config
- **Layer:** core
- **Plan:** docs/plan/phase-02-argument-parsing/2.3.3-parse-config.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/parsing/parse_config.c`, `coders/main.h`, and `coders/Makefile` compose numeric and scheduler parsing and return a bit mask for every invalid argument position.
- **Acceptance:** `tests/acceptance/test_2.3.3.c` — AC1–AC9 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.3.3.md`).
- **Documentation:** docs/reference/2.3.3-parse-config.md
- **Commit:** feat(parsing): parse configuration arguments [2.3.3]

## 2026-10-28 — Subtask 2.4.1: Print argument error
- **Layer:** interfaces
- **Plan:** docs/plan/phase-02-argument-parsing/2.4.1-print-arg-error.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/cli/print_arg_error.c`, `coders/main.h`, and `coders/Makefile` add position-specific argument diagnostics to the CLI layer.
- **Acceptance:** `tests/acceptance/test_2.4.1.c` — AC1–AC6 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.4.1.md`).
- **Documentation:** docs/reference/2.4.1-print-arg-error.md
- **Commit:** feat(interfaces): print argument errors [2.4.1]

## 2026-10-28 — Subtask 2.4.2: Validate arguments in main
- **Layer:** interfaces
- **Plan:** docs/plan/phase-02-argument-parsing/2.4.2-validate-arguments.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/main.c` validates all eight arguments through `parse_config`, reports each invalid field in argument order, prints usage once, and exits 1; `README.md` documents the rules and a multi-error run.
- **Acceptance:** `tests/acceptance/test_2.4.2.c` — AC1–AC7 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_2.4.2.md`).
- **Documentation:** docs/reference/2.4.2-validate-arguments.md
- **Commit:** feat(interfaces): validate arguments in main [2.4.2]

## 2026-10-28 — Subtask 3.1.1: Get time ms
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.1.1-get-time-ms.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/time/get_time_ms.c`, `coders/main.h`, and `coders/Makefile` add the wall-clock millisecond adapter; unit and acceptance coverage exercise its contract.
- **Acceptance:** `tests/acceptance/test_3.1.1.c` — AC1–AC4 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.1.1.md`).
- **Documentation:** docs/reference/3.1.1-get-time-ms.md
- **Commit:** feat(time): add millisecond clock [3.1.1]

## 2026-10-28 — Subtask 3.1.2: Get elapsed ms
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.1.2-get-elapsed-ms.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/time/get_elapsed_ms.c`, `coders/main.h`, and `coders/Makefile` add relative millisecond time with a future-start clamp and clock-failure propagation.
- **Acceptance:** `tests/acceptance/test_3.1.2.c` — AC1–AC5 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.1.2.md`).
- **Documentation:** docs/reference/3.1.2-get-elapsed-ms.md
- **Commit:** feat(time): add elapsed millisecond clock [3.1.2]

## 2026-10-28 — Subtask 3.2.1: Is stopped
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.2.1-is-stopped.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/state/is_stopped.c`, `coders/main.h`, and `coders/Makefile` add the mutex-protected stop-state reader; `docs/architecture.md` maps the state adapter.
- **Acceptance:** `tests/acceptance/test_3.2.1.c` — AC1–AC5 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.2.1.md`).
- **Documentation:** docs/reference/3.2.1-is-stopped.md
- **Commit:** feat(state): add stopped-state reader [3.2.1]

## 2026-10-28 — Subtask 3.2.2: Set stopped
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.2.2-set-stopped.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/state/set_stopped.c`, `coders/main.h`, and `coders/Makefile` add the mutex-coordinated stop writer using `log_mutex → stop_mutex` and reverse unlock attempts.
- **Acceptance:** `tests/acceptance/test_3.2.2.c` — AC1–AC7 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.2.2.md`).
- **Documentation:** docs/reference/3.2.2-set-stopped.md; task narrative: progress/pr_3.2.md
- **Commit:** feat(state): add synchronized stop transition [3.2.2]

## 2026-10-28 — Subtask 3.3.1: Sleep ms
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.3.1-sleep-ms.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/time/sleep_ms.c`, `coders/main.h`, and `coders/Makefile` add sliced sleeping that checks the shared stop state between waits.
- **Acceptance:** `tests/acceptance/test_3.3.1.c` — AC1–AC5 covered and green in the supplied pre-session check.
- **Verification:** supplied `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.3.1.md`); the backward-clock signed-overflow finding was fixed before approval.
- **Documentation:** docs/reference/3.3.1-sleep-ms.md; task narrative: progress/pr_3.3.md
- **Commit:** feat(time): add stop-aware sliced sleep [3.3.1]

## 2026-10-28 — Subtask 3.4.1: Build number text
- **Layer:** core
- **Plan:** docs/plan/phase-03-time-and-log/3.4.1-build-number-text.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/utils/build_number_text.c`, `coders/main.h`, and `coders/Makefile` add bounded, allocation-free decimal digit formatting for caller-owned buffers.
- **Acceptance:** `tests/acceptance/test_3.4.1.c` — AC1–AC7 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.4.1.md`).
- **Documentation:** docs/reference/3.4.1-build-number-text.md
- **Commit:** feat(utils): add decimal number text builder [3.4.1]

## 2026-10-28 — Subtask 3.4.2: Build log line
- **Layer:** core
- **Plan:** docs/plan/phase-03-time-and-log/3.4.2-build-log-line.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/log/build_log_line.c`, `coders/main.h`, and `coders/Makefile` add a pure formatter for one bounded serialized log line; `docs/architecture.md` maps the core `log/` module.
- **Acceptance:** `tests/acceptance/test_3.4.2.c` — AC1–AC6 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.4.2.md`).
- **Documentation:** docs/reference/3.4.2-build-log-line.md
- **Commit:** feat(core): build serialized log line [3.4.2]

## 2026-10-29 — Subtask 3.4.3: Print state
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.4.3-print-state.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/log/print_state.c`, `coders/main.h`, and `coders/Makefile` add the serialized state-line adapter; unit and acceptance coverage exercise its output, synchronization, and failure contracts.
- **Acceptance:** `tests/acceptance/test_3.4.3.c` — AC1–AC10 covered and green in the supplied check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.4.3.md`).
- **Documentation:** docs/reference/3.4.3-print-state.md
- **Commit:** feat(log): serialize state lines [3.4.3]

## 2026-10-29 — Subtask 3.4.5: Declare print log line
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.4.5-declare-print-log-line.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/main.h` declares the shared `print_log_line` adapter contract; no runtime behavior changed.
- **Acceptance:** `tests/acceptance/test_3.4.5.c` — AC1–AC2 cover the `print_state` regression and build/Norm requirements.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.4.5.md`).
- **Documentation:** docs/reference/3.4.5-declare-print-log-line.md
- **Commit:** feat(log): declare shared log writer [3.4.5]

## 2026-10-29 — Subtask 3.4.6: Extract print log line
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.4.6-extract-print-log-line.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/log/print_log_line.c` centralizes timestamp, formatting and output; `print_state.c` delegates the write while retaining its stop and serialization policy; `coders/Makefile` registers the shared writer.
- **Acceptance:** `tests/acceptance/test_3.4.6.c` — AC1–AC8 covered and green in the supplied close check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.4.6.md`).
- **Documentation:** docs/reference/3.4.6-extract-print-log-line.md
- **Commit:** refactor(log): extract shared log line writer [3.4.6]

## 2026-10-29 — Subtask 3.4.4: Print burnout
- **Layer:** adapters
- **Plan:** docs/plan/phase-03-time-and-log/3.4.4-print-burnout.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/log/print_burnout.c`, `coders/main.h`, and `coders/Makefile` add the serialized terminal log operation, which claims the first stop and delegates the line to `print_log_line`.
- **Acceptance:** `tests/acceptance/test_3.4.4.c` — AC1–AC12 cover output, stop ordering, concurrency, failure behavior, build integration, and Norm.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_3.4.4.md`).
- **Documentation:** docs/reference/3.4.4-print-burnout.md; task narrative: progress/pr_3.4.md
- **Commit:** feat(log): print terminal burnout line [3.4.4]

## 2026-10-29 — Subtask 4.1.1: Is request before
- **Layer:** core
- **Plan:** docs/plan/phase-04-priority-queue/4.1.1-is-request-before.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/policy/is_request_before.c`, `coders/main.h`, and `coders/Makefile` define and build the strict `(key, ticket)` request comparator; the architecture map documents the pure `policy/` module.
- **Acceptance:** `tests/acceptance/test_4.1.1.c` — AC1–AC9 cover ordering, edge cases, immutability, and build integration.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_4.1.1.md`).
- **Documentation:** docs/reference/4.1.1-is-request-before.md
- **Commit:** feat(policy): compare requests by priority [4.1.1]

## 2026-10-29 — Subtask 4.1.2: Get request key
- **Layer:** core
- **Plan:** docs/plan/phase-04-priority-queue/4.1.2-get-request-key.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/policy/get_request_key.c`, `coders/main.h`, and `coders/Makefile` add pure FIFO ticket and EDF deadline key calculation with invalid-input and overflow handling; `docs/architecture.md` maps key calculation in `policy/`.
- **Acceptance:** `tests/acceptance/test_4.1.2.c` — AC1–AC6 cover policy keys, validation, overflow boundaries, and build integration.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_4.1.2.md`).
- **Documentation:** docs/reference/4.1.2-get-request-key.md; task narrative: progress/pr_4.1.md
- **Commit:** feat(policy): compute FIFO and EDF request keys [4.1.2]

## 2026-10-30 — Subtask 4.2.1: Init heap
- **Layer:** core
- **Plan:** docs/plan/phase-04-priority-queue/4.2.1-init-heap.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/heap/init_heap.c`, `coders/main.h`, and `coders/Makefile` add fixed-capacity request storage with overflow-safe allocation sizing and unchanged heap fields on failure; `docs/architecture.md` maps the core heap module.
- **Acceptance:** `tests/acceptance/test_4.2.1.c` — AC1–AC6 cover initialization, slot writability, invalid inputs, independent allocations, and build integration.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_4.2.1.md`).
- **Documentation:** docs/reference/4.2.1-init-heap.md
- **Commit:** feat(heap): initialize priority queue storage [4.2.1]
