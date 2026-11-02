## Overview

ISS-14 changes `parse_config` to report invalid command-line arguments through a caller-owned `int invalid[9]` array and an invalid-argument count. `main` consumes those flags in argument order, preserving the existing error lines, usage output, and exit status. The change is scoped to the parser contract and its sole caller, with the old parser acceptance oracle migrated to the new API.

## Approach

The parser continues to validate numeric positions through `parse_field` and the scheduler through `parse_mode`; it writes a zero or one for every position 1 through 8 and counts failures without stopping early. Index 0 remains unused. `main` supplies stack storage and delegates each flagged position to the existing CLI error printer, keeping parsing pure and output in the interface layer.

## Decisions

The user chose one subtask for the parser contract and its caller: changing only the public prototype would leave the program uncompilable, while a split would require a temporary conversion back to the mask this task removes. The issue review also clarified that the public C API change is observable, so API criteria target the new contract while binary behavior remains a regression requirement.

## Issues resolved

The first issue review requested splitting parser and caller work, moving source-only checks out of acceptance criteria, fixing malformed pipe-delimited criteria, and correcting the structural-refactor classification. The approved spec resolves these by keeping the inseparable parser/caller change together, placing source constraints in `Code review check:`, using observable acceptance criteria, and explicitly treating the API change as observable. It migrates only `test_2.3.3.c`; the sealed binary oracle `test_2.4.2.c` remains unchanged.

## Issues open

Nothing open.

## Follow-ups

No follow-up is specified by the issue. The next project work remains task 4.3, beginning with 4.3.1 after the task PR is reviewed and merged.

## Branch notes

## Where to start reviewing

1. `coders/src/parsing/parse_config.c` — see how each argument becomes a flag and contributes to the returned count.
2. `coders/main.c` — follow the stack array from the parser call to ordered diagnostics.
3. `tests/acceptance/test_ISS-14.1.c` — review the API contract and unchanged binary behavior.
4. `tests/acceptance/test_2.3.3.c` — compare the migrated assertions with the former mask cases.
