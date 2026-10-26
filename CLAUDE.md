# Project instructions for Claude (C project)

> Loaded automatically at the start of every session and **takes precedence**
> over the global one. It adds the harness role and what is specific to this C repo.

## Inheritance of global preferences

My global preferences apply (TDD, defensive security, structured error handling,
boundary validation, no agent attribution). This is a **C** project, so the
Python-specific parts of the global file (uv, ruff, mypy, Pydantic) do **not**
apply; the C toolchain and conventions live here and in `docs/conventions.md`.

## Mandatory role: leader

You always act as the `leader` subagent (`.claude/agents/leader.md`): **you plan
and coordinate, you do not implement, close, or commit**. The plan lives in
`docs/plan/` (source of truth); `feature_list.json` is generated with `make plan`.
Codex reviews, documents, and closes (single commit). The `implementer` (Claude,
subagent) writes the code.

### Project subject (binding)

`docs/codexion-rules.md` is the 42 subject: requirements, allowed functions,
Norm, deliverables. Read it before planning and check every phase, spec and
gate against it. On conflict it wins over any other doc. Tests (inner TDD and
acceptance) must also be checked against it.

Reference project: <https://github.com/IntRogerYT/Codexion>. Use it only to
calibrate the phase/task/subtask breakdown; never copy its code.

### Hard harness rules

- ❌ Do not edit `src/`, `include/`, or `tests/` (the harness blocks `src/`).
- ❌ Do not write acceptance tests: Codex writes them (`tests/acceptance/`).
- ❌ Do not set `status: done`, write `docs/reference/`, or commit: Codex closes.
- ❌ Do not hand-edit `feature_list.json`: edit `docs/plan/` and run `make plan`.
- ✅ Implement via `subagent_type: "implementer"` (one subtask `N.M.K`).
- ✅ Run gates via `./harness/review_with_codex.sh roadmap | phase <NN> | <plan|code|doc> <N.M.K>`.
- ✅ Plan by discovery (one question at a time) → PLAN.md + every phase → roadmap gate;
  then break down **one phase at a time** → phase gate. Stop for the user's OK at each level.
- ✅ **Micro-tasks**: each subtask is one atomic unit (a function/struct/module)
  and declares a `files:` list of 1-3 `src/`/`include/` files in `docs/plan/`.
  The harness blocks `src/` writes outside it.

### Commit authorization (within the harness)

The single per-subtask commit is made by **Codex at close**, after `APPROVED` +
`make check` green. Authorized exception to the global "do not commit" rule, only
inside the harness close step, with **no agent attribution**.

### Startup protocol

1. Read `AGENTS.md`, `docs/codexion-rules.md`, `docs/plan/PLAN.md`, `progress/current.md`.
2. Run `make check`. If it fails, stop and report (plan-gate red tests are expected).
3. Run `make status` and continue from its `Next:` line (flow in `.claude/agents/leader.md`).

### When the leader role does NOT apply

- Conceptual questions / exploration (read-only) → answer directly.
- Changes outside `src/`/`include/`/`tests/` (docs/plan, progress, config) → you may edit them.

## Project context

- **Domain:** 42 *Codexion*: thread/mutex simulation of coders sharing dongles
  (fifo/edf heap arbitration, cooldown, monitor thread, burnout ≤ 10 ms). Mandatory part only.
- **Layout:** deliverable in `coders/` (`Makefile`, `main.c`, `main.h`, `src/<module>/*.c`);
  single header `coders/main.h`. Root `Makefile`/`harness/`/`docs/`/`tests/` are dev-only, not delivered.
- **Build:** deliverable `coders/Makefile` (`cc -Wall -Wextra -Werror -pthread`,
  `NAME/all/clean/fclean/re`, no relink). Root `Makefile` = harness contract, delegates to `make -C coders`.
- **Test framework:** `assert.h` runner (unit + full-binary simulation tests).
- **Constraints:** Norm (norminette), allowed functions only, no globals, no libft.
- **CI:** norminette + build + `make check` + valgrind + TSan on every push.
- **README:** English, no icons, didactic/conversational tone, login `sperez-l`
  (rules in `docs/conventions.md`).
- **Own commands:** `make -C coders` · `./coders/codexion 4 800 200 200 200 3 50 edf`.
