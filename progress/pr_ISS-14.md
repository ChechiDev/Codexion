## Overview

ISS-14 changes `parse_config` from returning a bit mask to filling a caller-owned `int invalid[9]` array and returning the number of invalid arguments. `main` consumes the flags in argument order, preserving the existing diagnostics, usage output, and exit status. The change updates the parser contract and its sole caller, and migrates the old parser acceptance oracle to the new API.

## Approach

The parser keeps its existing field-validation path: numeric positions go through `parse_field`, and the scheduler goes through `parse_mode`. It writes a zero or one for each position 1 through 8, preserves index 0, and counts all failures without stopping early. `main` owns stack storage and delegates each flagged position to the existing CLI diagnostic function. This keeps parsing pure and user-facing output in the interface layer.

## Decisions

The user chose one subtask for the parser contract and its caller. Changing only the public prototype would leave the program uncompilable; splitting the work would require temporary conversion back to the mask being removed. The issue review also established that the public C API change is observable, so the API has new acceptance criteria while the binary behavior remains a regression requirement.

## Issues resolved

The initial issue review requested splitting parser and caller work, moving source-only constraints out of acceptance criteria, fixing a delimiter-sensitive criterion, assigning each subtask its own acceptance path, and correcting the structural-refactor classification. The approved plan resolved these points by keeping the inseparable parser/caller change together, placing source checks in `Code review check:`, and testing the observable API and binary contracts. It migrated `tests/acceptance/test_2.3.3.c`; the sealed binary oracle `tests/acceptance/test_2.4.2.c` remains unchanged. The subtask plan review and code review found no further blocking issues.

## Issues open

Nothing open.

## Follow-ups

The issue specifies no follow-up work. Project work resumes at task 4.3, starting with subtask 4.3.1 after this task PR is reviewed and merged.

## Branch notes

## Where to start reviewing

1. `coders/src/parsing/parse_config.c` — see how each argument becomes a flag and contributes to the returned count.
2. `coders/main.c` — follow the stack array from the parser call to ordered diagnostics.
3. `tests/acceptance/test_ISS-14.1.c` — review the new API contract and unchanged binary behavior.
4. `tests/acceptance/test_2.3.3.c` — compare the migrated assertions with the prior mask cases.
