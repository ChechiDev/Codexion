# Review history: ISS-32

## Round 1: issue gate, CHANGES_REQUESTED (2026-11-06 11:16)

### Issue review — ISS-32

**Verdict:** CHANGES_REQUESTED

#### Analysis
- Coverage and size: one subtask is appropriately sized for the `sleep_ms` return-contract change. The declared production files are within scope, and dependencies 3.3.1 and 7.3.1 are done. The adapter layer is appropriate.
- Scope: the issue header says all four call sites in `coders/src/coder/run_coder.c` will be updated and lists that file under Expected files, but ISS-32.1 says the file must not change. The current call sites already handle both nonzero results alike (`coders/src/coder/run_coder.c:36,51,55,60`), so the no-edit approach is sound; the header must agree with it before the issue is sealed.
- Acceptance: AC1–AC4 observe the public `sleep_ms` contract. AC5 cannot establish its claimed coder regression: `coders/main.c:28-45` currently only parses arguments and exits, while phase 09 is not broken down. Both full-binary commands can pass without starting any coder, and the log and burnout conditions can pass vacuously. AC6 requires checking the text of another acceptance test, which is an oracle-migration procedure rather than observable product behavior; its build, symbol and Norm checks are suitable.
- Migration: `tests/acceptance/test_3.3.1.c` exists. Its AC3 and AC4 assert -1 for stops and must change to 1. Its other checks need no migration. The issue correctly leaves the sealed 3.3.1 spec as history.

#### Per subtask
- ISS-32.1: changes requested — correct the acceptance gap and align the issue header with the planned file scope.

#### Requested changes
1. In the issue header, remove the promised `run_coder.c` edits from Approach and Expected files, or revise the spec to declare and justify those edits. Keep the chosen behavior consistent throughout the header and spec.
2. Replace AC5's full-binary scenarios with an observable regression that runs `run_coder` through its public API using an initialized simulation and verifies interruption and shutdown, or defer the full-binary scenarios until phase 09 wires `main`. Ensure the criterion cannot pass when no coder runs.
3. Move AC6's assertion about the contents of `tests/acceptance/test_3.3.1.c` to the migration or code-review instructions. Keep AC6 as an observable build, symbol and Norm check, with one test in `test_ISS-32.1.c`.

## Round 2: issue gate, ISSUE_APPROVED (2026-11-06 11:29)

### Issue review — ISS-32

**Verdict:** ISSUE_APPROVED

#### Analysis
- Coverage and size: ISS-32.1 is one atomic change to `sleep_ms`'s return contract. It covers the issue goal without adding phase 09's failure-state behavior.
- Scope and architecture: the adapter-layer change declares two production files. `run_coder.c` is consistently excluded; its existing nonzero-result checks preserve coder behavior. The optional `main.h` touch is limited to contract documentation.
- Dependencies: 3.3.1 and 7.3.1 are done; there are no issue-local dependencies or cycles.
- Acceptance: AC1–AC4 check the observable return and timing contract. AC5 runs coder threads against an initialized simulation, checks real compile progress and shutdown, and cannot pass vacuously. AC6 checks the deliverable build, symbol, and Norm. All six criteria name the single issue oracle path.
- Migration: `tests/acceptance/test_3.3.1.c` exists and only its stop-result assertions in AC3 and AC4 need to change from -1 to 1. The spec states the superseded behavior and leaves the sealed 3.3.1 spec intact. No other sealed oracle needs migration.
- The stated `is_stopped` mutex-failure limitation matches the sealed 3.2.1 contract. Clock failures remain a code-review check because ordinary inputs cannot force them.

#### Per subtask
- ISS-32.1: ok — scope, dependencies, acceptance, and migration are consistent with the issue header and existing callers.
