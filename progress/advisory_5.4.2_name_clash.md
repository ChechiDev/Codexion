# 5.4.2 name clash — Codex advisory review (2026-11-03)

Advisory only (no gate, no seal). Prompt: init_sim prototype vs static init_sim helpers in 7 sealed oracles.

## Advisory review

**I recommend B, using `init_simulation`.** It keeps the permitted `init_` verb and describes the function’s job. The exact `init_sim`/`free_sim` pairing is appealing, but the current harness cannot carry out A: `migrates:` is frozen only at an **issue** gate. For a phase subtask, `_migrate.py` rejects it because there is no approved issue snapshot. Re-running phase 05 and plan 5.4.2 alone would therefore stop before review.

The risk with B is a change to the sealed 5.4.2 oracle and approved spec. Codex must update every call to the new public name, check that all six acceptance criteria and assertions remain intact, and re-seal through the gates. The seven older oracles need no edits. The implementer’s `setup_sim` renames in three **unit** tests are within scope: those tests belong to the implementer and need the same collision fix.

A third option is to extend the harness to support phase-subtask migrations, then use A. That is worthwhile only if this migration capability is wanted beyond 5.4.2; it is a separate harness change, not a step the present gates already support. Dropping the public prototype would evade the collision but would weaken the specified public API.

**Harness steps for B:**

1. Update the 5.4.2 spec to name `init_simulation` throughout, including its declared source file and prototype; keep the same behavior and criteria. Run `make plan`.
2. Run `./harness/review_with_codex.sh phase 05` to review and re-seal the changed spec.
3. At `./harness/review_with_codex.sh plan 5.4.2`, have Codex update the sealed `test_5.4.2.c` calls, verify its assertions, approve, and re-seal it.
4. Have the implementer align the source, `main.h`, Makefile, and unit test with the approved name. Keep the three unit-test helper renames.
5. Obtain a green outside-sandbox `make check` and the spec’s memory/thread checks, then run `./harness/review_with_codex.sh code 5.4.2`. Close only after approval and a green check.

**Recommendation: choose B with `init_simulation` under the harness as it exists.**
