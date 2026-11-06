## Overview

ISS-32 changes `sleep_ms` to report an observed simulation stop as `1`, a completed duration as `0`, and a clock failure as `-1`. This gives the upcoming failure-state work a way to distinguish normal shutdown from a real timing error while preserving the current coder behavior.

## Approach

The issue was delivered as one adapter subtask. The implementation changes only the existing stop branch in `sleep_ms`; clock failures, stop-first polling, duration handling, and bounded slices retain their established behavior. Existing `run_coder` call sites already leave on any nonzero result, so they needed no edits. Acceptance coverage checks the public return contract and exercises coder shutdown through an initialized simulation. The legacy 3.3.1 oracle was migrated for its two superseded stop assertions.

## Decisions

The issue review initially requested that the promised `run_coder.c` edits and regression criterion be reconciled with actual scope. The approved plan kept callers unchanged because their existing nonzero checks already handle both results, and replaced the regression with a non-vacuous public-API run that requires compile progress. The review also moved the legacy-oracle migration check into migration instructions. The `is_stopped` mutex-failure behavior remains governed by its existing contract and is therefore reported as a stop.

## Issues resolved

**Issue plan and acceptance gaps.** The initial issue review noted a scope mismatch around caller edits and that the proposed coder check could pass without a coder running; it also found a migration-procedure assertion in the acceptance criteria. These gaps preceded implementation because the issue header and first acceptance design did not yet reflect current caller behavior or require observable progress. Editing callers would add unnecessary scope, and retaining a vacuous test would not establish regression behavior. The issue plan was revised to leave `run_coder.c` unchanged, require real compile progress in its public-API check, and describe migration separately; the issue gate then approved it. No additional code review changes were requested.

## Issues open

Nothing open.

## Follow-ups

Phase 09 subtask 9.2.1 will use the new result to call `set_failed` only for a real `-1`; normal stop result `1` must remain a successful shutdown path.

## Branch notes

## Where to start reviewing

1. `coders/src/time/sleep_ms.c` — the changed stop result and preserved clock-error path.
2. `tests/acceptance/test_ISS-32.1.c` — public contract and coder-shutdown coverage.
3. `tests/acceptance/test_3.3.1.c` — the migrated assertions for the superseded contract.
4. `docs/plan/issues/ISS-32-refactor-sleep-ms-returns-1-on-stop-1-only-on-error-3-3/ISS-32.1-sleep-stop-result.md` — approved scope, criteria, and follow-up contract.
