## Overview

Task 2.4 completes entry-point validation for the eight command-line arguments. Invalid values now produce a field-specific error for every bad position, followed by one usage reminder and exit status 1. Valid input passes through without stderr output; simulation startup remains for a later phase.

## Approach

The entry point checks the argument count, delegates value rules and aggregation to `parse_config`, then walks its positional error mask from left to right and delegates each message to `print_arg_error`. This keeps parsing in the core and user-facing output in the CLI layer. The README now explains the accepted numeric and scheduler forms and shows a multi-error invocation.

## Decisions

The approved plan keeps the mask walk as a small static helper in `main.c`, since only the entry point needs it. The current successful path returns 0 without starting work; the spec reserves initialization, execution and teardown for phase 09.

## Issues resolved

At the phase review, the README requirement was moved from an acceptance criterion to a close obligation so the README could be updated after the code gate, in line with the repository workflow. During implementation, validation made the old exact-count unit fixture's placeholder arguments invalid; the fixture now supplies a valid configuration while retaining its count-focused assertion. The task 2.4 subtask reviews recorded no code findings.

## Issues open

Nothing open.

## Follow-ups

Phase 09 will replace the valid-input return path with initialization, simulation execution and teardown. The approved spec notes that input-normalization rules may change by later user decision.

## Branch notes

## Where to start reviewing

1. `coders/main.c` — shows the entry-point count check, parser call and ordered reporting flow.
2. `coders/src/cli/print_arg_error.c` — maps each invalid position to its diagnostic.
3. `coders/src/parsing/parse_config.c` — defines aggregation of all invalid argument positions.
4. `README.md` — gives the user-facing validation rules and multi-error example.
