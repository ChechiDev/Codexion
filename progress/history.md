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

## 2026-11-03 — Subtask 5.3.1: Init coder
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.3.1-init-coder.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/init/init_coder.c`, `coders/main.h`, and `coders/Makefile` add coder initialization with validated seat mapping, an owned mutex, zeroed compile state, and a borrowed simulation pointer.
- **Acceptance:** `tests/acceptance/test_5.3.1.c` — AC1–AC5 cover initialized fields, untouched thread storage, the one-coder mapping, invalid-input preservation, and mutex independence.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.3.1.md`); implementation notes report clean TSan, memcheck, and acceptance Valgrind runs.
- **Documentation:** docs/reference/5.3.1-init-coder.md
- **Commit:** feat(init): initialize coder state [5.3.1]

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

## 2026-10-30 — Subtask 4.2.2: Free heap
- **Layer:** core
- **Plan:** docs/plan/phase-04-priority-queue/4.2.2-free-heap.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/heap/free_heap.c`, `coders/main.h`, and `coders/Makefile` add idempotent heap teardown that releases the request array and resets the struct to NULL/0/0.
- **Acceptance:** `tests/acceptance/test_4.2.2.c` — AC1–AC6 cover empty, partial and full storage, repeated and NULL calls, reuse, and build integration.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_4.2.2.md`).
- **Documentation:** docs/reference/4.2.2-free-heap.md; task narrative: progress/pr_4.2.md
- **Commit:** feat(heap): release priority queue storage [4.2.2]

## 2026-11-02 — Subtask ISS-14.1: Report invalid arguments as flags
- **Layer:** core
- **Plan:** docs/plan/issues/ISS-14-refactor-replace-parse-config-bit-mask-with-invalid-flags-2-3/ISS-14.1-invalid-flags.md (plan_review: PLAN_APPROVED)
- **Changes:** `parse_config` now fills caller-owned per-position flags and returns an invalid count; `main.c` consumes the array, and the sealed 2.3.3 acceptance oracle was migrated from bit-mask assertions.
- **Acceptance:** `tests/acceptance/test_ISS-14.1.c` — AC1–AC6 covered and green in the supplied pre-session check.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_ISS-14.1.md`).
- **Documentation:** docs/reference/ISS-14.1-invalid-flags.md; updated historical notes: docs/reference/2.3.3-parse-config.md, docs/reference/2.4.1-print-arg-error.md, and docs/reference/2.4.2-validate-arguments.md; task narrative: progress/pr_ISS-14.md
- **Commit:** refactor(parsing): report invalid arguments as flags [ISS-14.1]

## 2026-11-02 — Subtask 4.3.1: Push request
- **Layer:** core
- **Plan:** docs/plan/phase-04-priority-queue/4.3.1-push-request.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/heap/push_request.c`, `coders/main.h`, and `coders/Makefile` add a bounded min-heap insertion operation that copies requests and sifts them upward using the shared comparator.
- **Acceptance:** `tests/acceptance/test_4.3.1.c` — AC1–AC9 covered and green in the supplied pre-session check.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_4.3.1.md`).
- **Documentation:** docs/reference/4.3.1-push-request.md
- **Commit:** feat(heap): add request insertion [4.3.1]

## 2026-11-02 — Subtask 4.3.2: Get head request
- **Layer:** core
- **Plan:** docs/plan/phase-04-priority-queue/4.3.2-get-head-request.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/heap/get_head_request.c`, `coders/main.h`, and `coders/Makefile` add a read-only heap operation that copies the minimum request to caller storage.
- **Acceptance:** `tests/acceptance/test_4.3.2.c` — AC1–AC7 cover head selection, immutability, failure behavior, copy semantics, and build integration.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_4.3.2.md`).
- **Documentation:** docs/reference/4.3.2-get-head-request.md
- **Commit:** feat(heap): add request head lookup [4.3.2]

## 2026-11-02 — Subtask 4.3.3: Pop request
- **Layer:** core
- **Plan:** docs/plan/phase-04-priority-queue/4.3.3-pop-request.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/heap/pop_request.c`, `coders/main.h`, and `coders/Makefile` add removal of the minimum request and restore heap order using the shared comparator.
- **Acceptance:** `tests/acceptance/test_4.3.3.c` — AC1–AC9 cover removal order, invariants, failure preservation, edge sizes, interleaving, and build integration.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_4.3.3.md`); implementation notes report the acceptance binary Valgrind exit 0.
- **Documentation:** docs/reference/4.3.3-pop-request.md; task narrative: progress/pr_4.3.md
- **Commit:** feat(heap): remove request head [4.3.3]

## 2026-11-02 — Subtask 5.1.1: Get dongle pair
- **Layer:** core
- **Plan:** docs/plan/phase-05-simulation-state/5.1.1-get-dongle-pair.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/init/get_dongle_pair.c`, `coders/main.h`, and `coders/Makefile` provide the pure circular seat mapping with ordered dongle indices.
- **Acceptance:** `tests/acceptance/test_5.1.1.c` — AC1–AC5 cover ring mapping, the single-dongle case, ordered pairs, `INT_MAX`, and invalid-input output preservation.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.1.1.md`); implementation notes report clean TSan and Valgrind runs.
- **Documentation:** docs/reference/5.1.1-get-dongle-pair.md; task narrative: progress/pr_5.1.md
- **Commit:** feat(init): map coder seats to ordered dongle pairs [5.1.1]

## 2026-11-02 — Subtask 5.2.1: Init dongle
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.2.1-init-dongle.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/init/init_dongle.c`, `coders/main.h`, and `coders/Makefile` initialize each dongle's mutex, condition variable, capacity-two request heap, and initial state with reverse rollback.
- **Acceptance:** `tests/acceptance/test_5.2.1.c` — AC1–AC6 cover defaults, synchronization, capacity/order, independent dongles, and NULL rejection.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.2.1.md`); implementation notes report clean ThreadSanitizer and Valgrind runs.
- **Documentation:** docs/reference/5.2.1-init-dongle.md
- **Commit:** feat(init): initialize dongle state [5.2.1]
# 2026-11-02 — Close 5.1.1 Get dongle pair

- Closed 5.1.1: added the reference note, marked the approved spec done, and refreshed the generated plan index.
- Updated `progress/current.md` and wrote the task 5.1 PR narrative for the harness.
- **Commit:** feat(init): map coder seats to ordered dongle pairs [5.1.1]

## 2026-11-03 — Subtask 6.1.1: Take ticket
- **Layer:** adapters
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.1.1-take-ticket.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/state/take_ticket.c`, `coders/main.h`, and `coders/Makefile` add synchronized allocation of bounded simulation-wide request tickets; `docs/architecture.md` records the counter in the state adapter.
- **Acceptance:** `tests/acceptance/test_6.1.1.c` — AC1–AC5 cover sequential tickets, bounds and exhaustion, concurrent uniqueness and order, and NULL inputs.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.1.1.md`); implementation notes report clean TSan and Valgrind runs.
- **Documentation:** docs/reference/6.1.1-take-ticket.md
- **Commit:** feat(state): allocate request tickets [6.1.1]

## 2026-11-03 — Subtask 6.1.2: Build request
- **Layer:** orchestration
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.1.2-build-request.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/dongle/build_request.c`, `coders/main.h`, and `coders/Makefile` combine the synchronized ticket adapter and FIFO/EDF request-key policy into one caller-owned request; `docs/architecture.md` maps the `dongle/` orchestration module.
- **Acceptance:** `tests/acceptance/test_6.1.2.c` — AC1–AC7 cover FIFO/EDF values, untouched state, invalid keys and exhausted tickets, concurrent request construction, and NULL inputs.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.1.2.md`); implementation notes report clean ThreadSanitizer and Valgrind runs.
- **Documentation:** docs/reference/6.1.2-build-request.md; task narrative: progress/pr_6.1.md
- **Commit:** feat(dongle): build FIFO and EDF requests [6.1.2]


## 2026-11-02 — Subtask 5.2.2: Destroy dongle
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.2.2-destroy-dongle.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/cleanup/destroy_dongle.c`, `coders/main.h`, and `coders/Makefile` add best-effort reverse-order teardown for a dongle's heap, condition variable, and mutex; `docs/architecture.md` documents the `cleanup/` adapter.
- **Acceptance:** `tests/acceptance/test_5.2.2.c` — AC1–AC5 cover teardown state, queued requests, used synchronization objects, reinitialization, and NULL rejection.
- **Verification:** supplied pre-session `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.2.2.md`); implementation review records clean TSan, memcheck, and direct acceptance Valgrind runs.
- **Documentation:** docs/reference/5.2.2-destroy-dongle.md
- **Commit:** feat(cleanup): destroy dongle resources [5.2.2]

## 2026-11-03 — Subtask 5.2.3: Free dongles
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.2.3-free-dongles.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/cleanup/free_dongles.c`, `coders/main.h`, and `coders/Makefile` add reverse-order teardown of the initialized dongle prefix and release its array; `docs/architecture.md` documents array ownership in cleanup.
- **Acceptance:** `tests/acceptance/test_5.2.3.c` — AC1–AC5 covered and green in the supplied close check.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.2.3.md`); implementation notes record clean TSan, memcheck, and acceptance Valgrind runs.
- **Documentation:** docs/reference/5.2.3-free-dongles.md
- **Commit:** feat(cleanup): release dongle array [5.2.3]

## 2026-11-03 — Subtask 5.2.4: Init dongles
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.2.4-init-dongles.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/init/init_dongles.c`, `coders/main.h`, and `coders/Makefile` add overflow-checked array allocation, ordered element initialization, and prefix rollback through `free_dongles`; acceptance and unit coverage exercise valid sizes, independence, and invalid inputs.
- **Acceptance:** `tests/acceptance/test_5.2.4.c` — AC1–AC4 covered and green in the supplied close check.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.2.4.md`); implementation notes record clean TSan, memcheck, and acceptance Valgrind runs.
- **Documentation:** docs/reference/5.2.4-init-dongles.md; task narrative: progress/pr_5.2.md
- **Commit:** feat(init): initialize dongle array [5.2.4]

## 2026-11-03 — Subtask 5.3.2: Destroy coder
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.3.2-destroy-coder.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/cleanup/destroy_coder.c`, `coders/main.h`, and `coders/Makefile` add coder mutex teardown; `docs/architecture.md` records it in the cleanup adapter map.
- **Acceptance:** `tests/acceptance/test_5.3.2.c` — AC1–AC4 cover destruction, preservation of other fields, reinitialization, and NULL rejection.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.3.2.md`); implementation notes report clean TSan, memcheck, and acceptance Valgrind runs.
- **Documentation:** docs/reference/5.3.2-destroy-coder.md
- **Commit:** feat(cleanup): destroy coder mutex [5.3.2]


## 2026-11-03 — Subtask 5.3.3: Free coders
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.3.3-free-coders.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/cleanup/free_coders.c`, `coders/main.h`, and `coders/Makefile` add reverse-order teardown of the initialized coder prefix and release its array; `docs/architecture.md` records coder arrays in cleanup ownership.
- **Acceptance:** `tests/acceptance/test_5.3.3.c` — AC1–AC5 cover full and partial arrays, zero count, NULL input, and negative-count preservation.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.3.3.md`); implementation notes report clean TSan, memcheck, and acceptance Valgrind runs.
- **Documentation:** docs/reference/5.3.3-free-coders.md
- **Commit:** feat(cleanup): release coder array [5.3.3]

## 2026-11-03 — Subtask 5.3.4: Init coders
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.3.4-init-coders.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/init/init_coders.c`, `coders/main.h`, and `coders/Makefile` add overflow-checked coder-array construction, ordered per-coder initialization, and prefix rollback through `free_coders`.
- **Acceptance:** `tests/acceptance/test_5.3.4.c` — AC1–AC4 cover initial state and ring mapping, one and large arrays, independent mutexes, cleanup, and invalid-input output preservation.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.3.4.md`); implementation notes record clean TSan and Valgrind runs, including the acceptance binary.
- **Documentation:** docs/reference/5.3.4-init-coders.md; task narrative: progress/pr_5.3.md
- **Commit:** feat(init): initialize coder array [5.3.4]

## 2026-11-03 — Subtask 5.4.1: Free sim
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.4.1-free-sim.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/cleanup/free_sim.c`, `coders/main.h`, and `coders/Makefile` add full simulation teardown; `docs/architecture.md` records the cleanup contract.
- **Acceptance:** `tests/acceptance/test_5.4.1.c` — AC1–AC6 cover full and partial resources, retained state, reuse, one coder, and NULL input.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.4.1.md`); implementation notes report clean TSan, memcheck, and acceptance Valgrind runs.
- **Documentation:** docs/reference/5.4.1-free-sim.md
- **Commit:** feat(cleanup): release simulation resources [5.4.1]

## 2026-11-03 — Subtask 5.4.2: Init simulation
- **Layer:** adapters
- **Plan:** docs/plan/phase-05-simulation-state/5.4.2-init-sim.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/init/init_simulation.c`, `coders/main.h`, and `coders/Makefile` build the complete simulation state in dependency order; failures use reverse rollback through `free_sim`. `docs/architecture.md` records the initialization contract.
- **Acceptance:** `tests/acceptance/test_5.4.2.c` — AC1–AC6 cover complete state, boundary counts, state API integration, sentinel storage, invalid input, and reuse.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_5.4.2.md`); implementation notes report clean TSan and memcheck. The review record notes no evidence for the separate acceptance-binary Valgrind run.
- **Documentation:** docs/reference/5.4.2-init-sim.md; task narrative: progress/pr_5.4.md
- **Commit:** feat(init): initialize simulation state [5.4.2]

## 2026-11-03 — Subtask 6.2.1: Is dongle ready
- **Layer:** core
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.2.1-is-dongle-ready.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/policy/is_dongle_ready.c`, `coders/main.h`, and `coders/Makefile` add the pure per-dongle readiness predicate; `docs/architecture.md` maps it in `policy/`.
- **Acceptance:** `tests/acceptance/test_6.2.1.c` — AC1–AC7 cover queue-head eligibility, availability, cooldown boundaries and overflow, immutability, and invalid inputs.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.2.1.md`); implementation notes report clean TSan and Valgrind runs.
- **Documentation:** docs/reference/6.2.1-is-dongle-ready.md
- **Commit:** feat(policy): add dongle readiness predicate [6.2.1]

## 2026-11-03 — Subtask 6.2.2: Get deadline
- **Layer:** adapters
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.2.2-get-deadline.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/time/get_deadline.c`, `coders/main.h`, and `coders/Makefile` add absolute deadline calculation for condition waits; `docs/architecture.md` maps elapsed clock and deadline helpers in `time/`.
- **Acceptance:** `tests/acceptance/test_6.2.2.c` — AC1–AC6 cover zero and positive delays, normalization, timed-wait compatibility, invalid inputs, and a large representable deadline.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.2.2.md`); implementation notes report clean TSan and Valgrind runs.
- **Documentation:** docs/reference/6.2.2-get-deadline.md; task narrative: progress/pr_6.2.md
- **Commit:** feat(time): calculate condition deadline [6.2.2]

## 2026-11-04 — Subtask 6.3.1: Wake dongles
- **Layer:** orchestration
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.3.1-wake-dongles.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/dongle/wake_dongles.c`, `coders/main.h`, and `coders/Makefile` add a best-effort broadcast across each dongle condition variable; `docs/architecture.md` records the stop wake-up responsibility.
- **Acceptance:** `tests/acceptance/test_6.3.1.c` — AC1–AC6 cover waking all dongles, multiple waiters, stop wake-up races, preserved state, the one-coder case, and invalid input.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.3.1.md`); implementation notes report clean TSan and acceptance Valgrind runs.
- **Documentation:** docs/reference/6.3.1-wake-dongles.md
- **Commit:** feat(dongle): wake dongle waiters [6.3.1]

## 2026-11-04 — Subtask 6.3.2: Wait for dongles
- **Layer:** orchestration
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.3.2-wait-for-dongles.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/dongle/wait_for_dongles.c`, `coders/main.h`, and `coders/Makefile` add a bounded condition wait that releases the second dongle mutex, waits on the first, and restores lock ownership in index order.
- **Acceptance:** `tests/acceptance/test_6.3.2.c` — AC1–AC4 cover timed waiting, availability of the second mutex during the wait, repeated use, and invalid inputs.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.3.2.md`); implementation notes report clean Valgrind and TSan runs.
- **Documentation:** docs/reference/6.3.2-wait-for-dongles.md
- **Commit:** feat(dongle): wait for dongles [6.3.2]


## 2026-11-04 — Subtask 6.3.3: Release dongles
- **Layer:** orchestration
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.3.3-release-dongles.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/dongle/release_dongles.c`, `coders/main.h`, and `coders/Makefile` release a coder's pair under ordered locks, timestamp cooldown after lock acquisition, notify waiters, and handle the single-dongle case; `docs/architecture.md` records the module contract.
- **Acceptance:** `tests/acceptance/test_6.3.3.c` — AC1–AC6 cover release timing, both wakeups, queue preservation and cooldown, one coder, invalid inputs, and lock contention.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.3.3.md`); implementation notes report clean Valgrind and TSan runs for the acceptance binary and clean Valgrind for the unit binary.
- **Documentation:** docs/reference/6.3.3-release-dongles.md
- **Commit:** feat(dongle): release dongles with cooldown [6.3.3]

## 2026-11-04 — Subtask 6.3.4: Take dongles
- **Layer:** orchestration
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.3.4-take-dongles.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/dongle/take_dongles.c`, `coders/main.h`, and `coders/Makefile` add atomic two-dongle acquisition using the same scheduled request in both heaps; queued fatal failures stop the simulation and wake waiters.
- **Acceptance:** `tests/acceptance/test_6.3.4.c` — AC1–AC7 cover acquisition, head-of-both ordering, FIFO/EDF priority, cooldown, stop, progress evidence, and invalid inputs.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.3.4.md`); implementation notes report clean acceptance Valgrind and corrected-oracle TSan runs.
- **Documentation:** docs/reference/6.3.4-take-dongles.md
- **Commit:** feat(dongle): acquire both dongles atomically [6.3.4]

## 2026-11-04 — Subtask 6.3.5: Take single dongle
- **Layer:** orchestration
- **Plan:** docs/plan/phase-06-dongle-arbitration/6.3.5-take-single-dongle.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/dongle/take_single_dongle.c`, `coders/main.h`, and `coders/Makefile` add one-coder acquisition through a single queued request, mutex, and cooldown check; `docs/architecture.md` records the single-dongle orchestration path.
- **Acceptance:** `tests/acceptance/test_6.3.5.c` — AC1–AC5 cover acquisition, FIFO/EDF cooldown, one queued waiter, stop, and invalid input.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_6.3.5.md`); implementation notes report clean acceptance Valgrind and TSan runs.
- **Documentation:** docs/reference/6.3.5-take-single-dongle.md; task narrative: progress/pr_6.3.md
- **Commit:** feat(dongle): acquire single dongle [6.3.5]


## 2026-11-04 — Subtask 7.1.1: Set compile start
- **Layer:** orchestration
- **Plan:** docs/plan/phase-07-coder-thread/7.1.1-set-compile-start.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/coder/set_compile_start.c`, `coders/main.h`, and `coders/Makefile` add the per-coder timestamp update under its mutex; `docs/architecture.md` records the coder module.
- **Acceptance:** `tests/acceptance/test_7.1.1.c` — AC1–AC5 cover elapsed timestamp storage, overwrite, future-start clamp, mutex contention, and invalid inputs.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_7.1.1.md`); implementation notes report clean Valgrind and TSan runs.
- **Documentation:** docs/reference/7.1.1-set-compile-start.md
- **Commit:** feat(coder): set compile start [7.1.1]

## 2026-11-04 — Subtask 7.1.2: Set compile count
- **Layer:** orchestration
- **Plan:** docs/plan/phase-07-coder-thread/7.1.2-set-compile-count.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/coder/set_compile_count.c`, `coders/main.h`, and `coders/Makefile` add a bounded completed-compile increment protected by the coder mutex; `docs/architecture.md` documents both coder state updates.
- **Acceptance:** `tests/acceptance/test_7.1.2.c` — AC1–AC5 cover increments, bounds, concurrency, mutex contention, and NULL input.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_7.1.2.md`); implementation notes report clean acceptance Valgrind and ThreadSanitizer runs.
- **Documentation:** docs/reference/7.1.2-set-compile-count.md; task narrative: progress/pr_7.1.md
- **Commit:** feat(coder): set compile count [7.1.2]

## 2026-11-05 — Subtask 7.2.1: Take coder dongles
- **Layer:** orchestration
- **Plan:** docs/plan/phase-07-coder-thread/7.2.1-take-coder-dongles.md (plan_review: PLAN_APPROVED)
- **Changes:** `coders/src/coder/take_coder_dongles.c`, `coders/main.h`, and `coders/Makefile` add pair/single acquisition orchestration, per-dongle acquisition logs, and one release attempt after a log failure; `docs/architecture.md` records the coder module's responsibility and dependencies.
- **Acceptance:** `tests/acceptance/test_7.2.1.c` — AC1–AC6 cover pair and single acquisition, stop, log failure, invalid input, and ticket exhaustion.
- **Verification:** supplied outside-sandbox `make check` green (exit code 0). Code review: APPROVED (`progress/review_7.2.1.md`); implementation notes report clean acceptance Valgrind and ThreadSanitizer runs.
- **Documentation:** docs/reference/7.2.1-take-coder-dongles.md
- **Commit:** feat(coder): take and log coder dongles [7.2.1]
