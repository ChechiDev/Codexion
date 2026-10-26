# CHECKPOINTS — Final-state evaluation (C)

> Objective criteria that Codex walks through marking `[x]`/`[ ]` before closing
> a subtask `N.M.K`. It rejects the close if any box in C1–C6 is left empty.

## C1 — The harness is complete
- [ ] Base files: `AGENTS.md`, `CLAUDE.md`, `Makefile`, `CHECKPOINTS.md`, `docs/plan/PLAN.md`.
- [ ] Docs: `docs/architecture.md`, `docs/conventions.md`, `docs/verification.md`.
- [ ] Symlinks installed: `.claude/agents`, `.claude/codex`, `harness/`; dirs `docs/plan`, `docs/reference`.
- [ ] `make check` finishes with exit 0 (includes plan↔feature_list sync).

## C2 — The state is consistent
- [ ] At most one leaf `in_progress` in the plan.
- [ ] `feature_list.json` is in sync with `docs/plan/` (`make plan` not pending).
- [ ] The worked subtask has `plan_review_<N.M.K>.md` = `PLAN_APPROVED`.
- [ ] Every `done` leaf has `review_<N.M.K>.md` = `APPROVED`.
- [ ] `progress/current.md` describes the active session (no leftover junk).

## C3 — The architecture is respected
- [ ] `core` code does not `#include` adapter/platform headers (deps point inward).
- [ ] Public API only in `include/<name>/`; opaque types where appropriate.
- [ ] Dependencies injected (context struct / function pointers); no global mutable state.
- [ ] No circular includes; one responsibility per translation unit.

## C4 — Quality and security
- [ ] `make verify` green: warning-clean compile (`-Wall -Wextra -Werror -pedantic`) + tests.
- [ ] **Clean code / SOLID**: single responsibility; no monster functions; **DRY**.
- [ ] **Micro-task scope**: only the subtask's declared `files:` (1-3 under `src/`/`include/`) were touched.
- [ ] Explicit error handling; every allocation checked and freed on all paths (no leaks).
- [ ] Bounds-checked buffers (no `gets`/unbounded `strcpy`/`sprintf`); external input validated.
- [ ] No secrets in code, no logging of sensitive data.

## C5 — Verification is real
- [ ] `tests/acceptance/test_<N.M.K>.c` exists, exercises only the public API, and passes.
- [ ] **1:1 traceability**: each criterion `ACn` has its check. No orphan criterion.
- [ ] There are `tests/unit/` covering the design; they are not trivial.
- [ ] (When tools exist) ASan/UBSan or Valgrind clean on the new tests.

## C6 — The close is correct
- [ ] `docs/reference/<N.M.K>-<name>.md` written with the required structure:
      frontmatter (`id`, `alias`, `tags`, `date`) + the five `<!-- section: … -->`
      anchors (concept/planning/execution/problems/files). `doc_gate` must pass.
- [ ] `status: done` set in `docs/plan/`; `feature_list.json` regenerated.
- [ ] `progress/history.md` has an entry for the subtask.
- [ ] Single commit: `<type>(<scope>): <what> [N.M.K]` + `Plan:` footer, **no agent attribution**.
