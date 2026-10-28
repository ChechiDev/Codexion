## Overview

Task 2.3 adds the core configuration parsing layer for Codexion. It centralizes numeric argument minimums, recognizes the exact scheduler names, and parses all eight values into `t_config` while reporting every invalid argument position in a bit mask. The task gives the upcoming CLI validation work a stable parser and shared validation rules.

## Approach

The parser is composed from small core functions: numeric text is handled by `parse_number`, positional lower bounds come from `get_arg_minimum`, and the scheduler is delegated to `parse_mode`. `parse_config` applies the numeric rule to positions 1 through 7, checks position 8, and accumulates one error bit per invalid position. This keeps the parsing layer free of output and platform work while allowing the interface to report all errors in order.

## Decisions

The scheduler must match lowercase `fifo` or `edf` exactly, including no surrounding whitespace, following the subject. The positional minimum lookup is shared so validation and later CLI diagnostics use the same lower bounds. The parser checks all fields in one call and returns a bit mask, as specified for multi-error reporting.

## Issues resolved

The roadmap and phase reviews resolved earlier plan issues around exact scheduler matching, shared-file dependency order, acceptance traceability, and the trimming helper's module boundaries. During 2.3.3 code review, the static helper name was revised to use the required verb prefix; the re-review approved the result.

## Issues open

Nothing open.

## Follow-ups

Task 2.4 consumes the mask to print one position-specific error for every invalid argument, then validates arguments in `main` before starting simulation work. The configuration contents are unspecified when `parse_config` returns a nonzero mask and must not be consumed.

## Branch notes

## Where to start reviewing

1. `docs/plan/phase-02-argument-parsing/2.3.3-parse-config.md` — the field mapping, mask contract, and acceptance criteria.
2. `coders/src/parsing/parse_config.c` — how the shared parsing rules are composed and failures accumulated.
3. `coders/src/parsing/get_arg_minimum.c` and `coders/src/parsing/parse_mode.c` — the numeric minimum and exact scheduler contracts used by the parser.
