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

## Round 4: issue-draft gate, CHANGES_REQUESTED (2026-11-06 12:43)

### Issue draft review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Problem: concrete and supported by the current `print_state` return path and the planned phase 09 failure flag.
- Template: all required sections are present and describe intended work.
- Labels: exactly one type label (`refactor`); priority `now` is valid.
- Size: one production function and its affected tests fit one issue. No agent attribution appears in the draft.
- The sealed-oracle migration list is incomplete. The draft names the post-stop assertion in AC7 of `test_3.4.3.c`, but misses the loop condition that also depends on a stop returning -1.

#### Requested changes
1. In `progress/issue_draft.md:51-56`, include `tests/acceptance/test_3.4.3.c:436` in the AC7 migration and describe changing that loop's stop condition to recognize result 1. With only the listed assertion at line 445 changed, the loop keeps treating 1 as a successful write, increments its count, and does not terminate after the stop.

## Round 5: issue-draft gate, DRAFT_APPROVED (2026-11-06 12:48)

### Issue draft review

**Verdict:** DRAFT_APPROVED

#### Analysis
- Problem: the existing `print_state` stop path returns -1, as do real failures. The draft explains the resulting ambiguity for phase 09's planned failure flag and identifies the current callers.
- Template: every section of `harness/issue_template.md` is present. Intended changes use future tense; statements about current behaviour use present tense.
- Labels: exactly one type label (`refactor`); priority `now` is valid.
- Size: one production function, its unit test, and four affected sealed oracles fit one issue. The migration list includes both stop-sensitive worker loops and their post-stop assertions. No agent attribution appears in the draft.

## Round 6: issue-draft gate, CHANGES_REQUESTED (2026-11-10 15:04)

### Issue draft review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Problem: the draft gives a concrete at-bound EDF failure, reproduction command, expected and actual results, and its impact. The stated 1610 ms boundary matches the formula in `docs/plan/PLAN.md`.
- Template / labels / size: all template sections are present; `bug` is the sole type label and `now` is a valid priority. Diagnosis followed by a focused correction fits one issue.
- Gate blocker: the supplied pre-session `make check` exited 2. `tests/acceptance/test_9.4.1.c:287` failed after the run logged a burnout at 1000 ms instead of the asserted 300–310 ms range. This is a different oracle from the proposed issue. The reviewer role's hard rule forbids approval while the check is red.

#### Requested changes
1. Resolve the `test_9.4.1.c:287` failure and provide a green outside-sandbox `make check` result before rerunning the issue-draft gate. The draft itself needs no content change based on this review.

## Round 7: issue-draft gate, DRAFT_APPROVED (2026-11-10 15:26)

### Issue draft review

**Verdict:** DRAFT_APPROVED

#### Analysis
- Problem: `progress/issue_draft.md:8-49` identifies the intermittent EDF burnout at the claimed 1610 ms viability boundary, gives a reproduction command, distinguishes expected from actual behavior, and explains the effect on the sealed oracle and CI. The boundary matches `docs/plan/PLAN.md:92-96`. The rare late burnout is recorded as a possibly related symptom for diagnosis.
- Template: all sections of `harness/issue_template.md` are present. The Overview and Approach describe intended investigation and correction; open questions preserve the unresolved root cause. No agent attribution appears in the draft.
- Labels: the front matter has exactly one type label (`bug`) and a valid priority (`now`).
- Size: diagnosis and a focused fix to the viability claim or its implementation fit one issue. The draft calls for one subtask per affected unit and a roadmap re-seal plus explicit oracle migration if the claim changes.
- Gate: the supplied outside-sandbox pre-session `make check` exited 0.

## Round 8: issue-draft gate, CHANGES_REQUESTED (2026-11-11 16:53)

### Issue draft review

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Problem: the missed burnout and target-priority race are concrete and supported by `coders/src/coder/set_compile_start.c:30`, `coders/src/monitor/get_burned_out.c:25-32`, and `coders/src/monitor/run_monitor.c:68-72`. The draft gives expected and actual behavior and a deterministic reproduction approach.
- Template and labels: all required sections and front matter are present; `bug` is the sole type label and `now` is a valid priority. The intended-work sections need future-tense wording.
- Size: the burnout fix fits one issue. The README AI-use correction is a separate documentation concern, so including it expands this issue beyond one task.
- Attribution: `progress/issue_draft.md:8-10` names the reviewing agents, contrary to the issue-draft rule forbidding agent attribution.

#### Requested changes
1. Remove the attribution at `progress/issue_draft.md:8-10`; retain the concrete defect and code evidence.
2. Keep this issue focused on missed burnout. Move the unrelated README correction at lines 34-36, 41-42, 52, 65, 75, 85, and 89-92 to a separate documentation issue or the existing README work. Update the affected sections consistently.
3. Write the intended work in future tense, especially the `Approach` and `Decisions` bullets at lines 46-58. Correct the `set_compile_start` spec reference in `Documentation`: it is 7.1.1, not 7.2.x.

## Round 9: issue-draft gate, DRAFT_APPROVED (2026-11-11 16:57)

### Issue draft review

**Verdict:** DRAFT_APPROVED

#### Analysis
- Problem: the draft identifies two concrete ways an expired burnout can be missed, cites the relevant code and subject clauses, and gives a deterministic reproduction with expected and actual behavior.
- Template: every section of `harness/issue_template.md` is present. The proposed work is in future tense, and the documentation references the correct existing specs and potentially affected sealed oracles.
- Labels: `bug` is the sole type label; `now` is a valid priority. The draft contains no agent attribution.
- Size: preserving an expired deadline and checking burnout before the compile target form one focused issue, with two plausible subtasks.
