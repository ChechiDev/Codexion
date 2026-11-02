# ISS-14 refactor: replace parse_config bit mask with invalid flags [2.3]
- issue: https://github.com/ChechiDev/codexion/issues/14
- type: refactor
- origin: 2.3
- priority: next
- goal: parse_config reports invalid arguments through an int invalid[9] flag array instead of a bit mask, with no change in user-visible behaviour

## Issue body (imported from GitHub)

## Overview

`parse_config` (task 2.3.3) reports invalid arguments as a bit mask: bit `n`
(`1 << n`) is set for each invalid argument `n` in 1..8, and `main` walks it
with `mask & (1 << position)`. It works and is tested, but it asks the reader
(and the evaluator) to know bitwise operators to follow plain argument
validation. This issue replaces the mask with an explicit array of flags,
with no change in user-visible behaviour.

## Approach

`parse_config` gains an out-parameter `int *invalid` (9 ints, index 0 unused)
and returns how many arguments are invalid (0 = valid config):

```c
int	parse_config(char **argv, t_config *config, int *invalid);
```

- The loop runs positions 1..8 and writes `invalid[position]` as 0 or 1 for
  every position, so the caller does not need to zero the array first.
- Positions 1..7 keep going through `static parse_field` (`parse_number` +
  `get_arg_minimum`); position 8 through `parse_mode`. No validation rule changes.
- Every argument is still checked (no early stop), so the user still sees
  all mistakes at once.
- `main` declares `int invalid[9]` on the stack; `print_arg_errors` tests
  `invalid[position]` instead of `mask & (1 << position)`.

## Decisions

- Structural refactor only: same accepted/rejected inputs, same error lines,
  same order, same usage and exit status 1.
- Array instead of the mask: one more parameter in exchange for code that
  reads without bitwise knowledge. No allocation is added (stack array).
- `parsing/` stays pure: it still prints nothing; `cli/` owns the messages.

## Issues

### Open

- `tests/acceptance/test_2.3.3.c` (sealed, Codex-owned) asserts the mask
  values (`1 << pos`, `0x10C`, `0x1FE`). It must be migrated at this task's
  plan gate to the equivalent `invalid[]` checks and re-sealed, the same way
  earlier renames migrated their oracles (1.2.2, 2.2.2, 2.2.7).
- `tests/unit/test_parse_config.c` (Claude-owned) must be rewritten for the
  new signature.

## Follow-ups

None expected beyond the test migration.

## Subtasks

To be planned in `docs/plan/` when the work starts (likely one subtask for
`parse_config` + its prototype, one for `main.c`; split if a gate requires it).

## Files

Expected to change:

| Status | File |
|---|---|
| Modified | `coders/src/parsing/parse_config.c` |
| Modified | `coders/main.h` |
| Modified | `coders/main.c` |
| Modified | `tests/acceptance/test_2.3.3.c` (gate migration) |
| Modified | `tests/unit/test_parse_config.c` |

Unchanged regression oracle: `tests/acceptance/test_2.4.2.c` (full-binary
error output).

## Documentation

Current specs: `docs/plan/phase-02-argument-parsing/2.3.3-parse-config.md`,
`docs/plan/phase-02-argument-parsing/2.4.2-validate-arguments.md`.

## Branch and merge

To be done after task 4.2, on its own branch (e.g.
`refactor/parse-config-flags`), not mixed with phase 04 work. Merging the PR
is the user's call.

## Where to start reviewing

1. `coders/src/parsing/parse_config.c` — the loop that fills `invalid[]`.
2. `coders/main.c` — `print_arg_errors` and the call site.
3. `tests/acceptance/test_2.3.3.c` — check the migrated assertions map
   one-to-one to the old mask checks.
