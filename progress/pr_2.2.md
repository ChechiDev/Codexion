## Overview

Task 2.2 applies the project conventions to the parsing and interface foundation. It puts usage output in the CLI layer, organizes the shared header, settles on a borrowed slice API, and moves reusable whitespace and digit rules into `utils/`. The final parser change reuses those helpers without changing numeric input behavior.

## Approach

The task makes the conventions changes in dependency order. It first moves user-facing output and organizes declarations, then migrates trimming from the legacy span to the slice view and removes the obsolete API. Small pure predicates define whitespace and decimal digits; trimming and numeric parsing call those shared helpers. Regression oracles preserve the public contracts through the structural steps.

## Decisions

The behavior-preserving parser and trimming substitutions use the structural-refactor gate exception: public acceptance tests remain regression oracles, and the code review check inspects the intended helper reuse. The slice remains a non-owning view, so these changes do not introduce copies or ownership transfer.

## Issues resolved

The plan review for 2.2.5 initially found that its stated behavior could not produce a red public-API test because the behavior already existed. The spec was revised to state the structural-refactor exception and use source inspection at code review for the helper substitution. During 2.2.7 planning, review also found older sealed acceptance oracles still referring to the retired span API or source path; those assertions were migrated to the surviving slice API and resealed. The 2.2.9 acceptance oracle's expected leading offset was corrected from six to seven after review of its input.

## Issues open

Nothing open.

## Follow-ups

Task 2.2 leaves the completed slice and generic predicates available to the config parser in task 2.3. That later task applies field-specific minimums and exact scheduler matching while composing the existing numeric parser.

## Branch notes

## Where to start reviewing

1. `coders/src/utils/trim_slice.c` — shows the shared whitespace predicate used by the trimming scan.
2. `coders/src/parsing/parse_number.c` — shows the parser consuming the borrowed slice and shared digit predicate while retaining its range contract.
3. `coders/main.h` — shows the final public declarations grouped by module.
