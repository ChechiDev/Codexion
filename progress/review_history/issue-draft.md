# Review history: draft

## Round 1: issue-draft gate, CHANGES_REQUESTED (2026-11-06 10:45)

### Issue draft review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Problem: the draft identifies the ambiguous `sleep_ms` result and its effect on phase 09. The current caller and sealed AC3/AC4 support that account.
- Template: all sections of `harness/issue_template.md` are present and describe intended work.
- Labels: exactly one type label (`refactor`); priority `now` is valid.
- Size: the contract change, caller handling and oracle migration fit one issue.
- Contract: `progress/issue_draft.md:21-28` promises `-1` on errors and `1` on stop, but `coders/src/state/is_stopped.c:19-23` reports mutex lock/unlock failure as `1`. Changing only the stop branch in `sleep_ms` would therefore report that error as a normal stop. The draft does not specify how to preserve the promised distinction.
- Attribution: `progress/issue_draft.md:14-15` says “Codex advisory,” contrary to the issue-draft rule barring agent attribution in published text.

#### Requested changes
1. Specify how `sleep_ms` will distinguish a real stop from a stop-mutex failure while retaining `-1` for errors. Include any necessary source/API changes in the expected scope and cover the error path in the issue's acceptance strategy.
2. Remove “Codex advisory” from the issue body. A neutral reference to the phase 09 decision is sufficient.

## Round 2: issue-draft gate, CHANGES_REQUESTED (2026-11-06 10:48)

### Issue draft review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Problem: concrete and supported by `sleep_ms` and `run_coder`; the distinction matters for the planned phase 09 failure flag.
- Template: all required sections are present, but several descriptions of intended changes use present tense rather than the future tense required by the issue-draft gate. The Documentation entry also leaves the status of the sealed 3.3.1 spec unclear.
- Labels: exactly one valid type label (`refactor`); `priority: now` is valid.
- Size: one return-contract change, its callers, and one oracle migration fit one issue. No agent attribution appears in the draft.

#### Requested changes
1. Rewrite intended work in future tense, especially the Overview (`progress/issue_draft.md:19-28`), Decisions (`progress/issue_draft.md:48-59`), and Follow-ups (`progress/issue_draft.md:68`). Present-tense statements of the current problem may remain.
2. Clarify at `progress/issue_draft.md:87` that the sealed `docs/plan/phase-03-time-and-log/3.3.1-sleep-ms.md` remains a historical record; the new issue spec will state that it supersedes AC3 and AC4, and only the listed sealed acceptance test will be migrated. Approved plan edits would otherwise require a new gate.

## Round 3: issue-draft gate, DRAFT_APPROVED (2026-11-06 10:54)

### Issue draft review

**Verdict:** DRAFT_APPROVED

#### Analysis
- Problem: the current `sleep_ms` return value conflates a normal stop with a clock failure, and `run_coder` treats both as failure. The draft explains the effect this would have on the planned phase 09 failure flag.
- Template: every section of `harness/issue_template.md` is present. Intended work is described in future tense; the existing behavior is stated in present tense.
- Labels: exactly one type label (`refactor`); `priority: now` is valid.
- Size: the return-contract change, caller handling, unit test update, and migration of one sealed acceptance oracle fit one issue. The draft leaves the sealed 3.3.1 spec unchanged and identifies the superseded criteria. It explicitly retains the existing `is_stopped` mutex-failure limitation. No agent attribution appears.
