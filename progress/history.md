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
