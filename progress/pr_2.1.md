## Overview

Task 2.1 adds the two reusable numeric parsing primitives used by later argument validation. `trim_span` returns a non-copying view after surrounding whitespace is removed, and `parse_number` converts the resulting ASCII digit span to an `int` without accepting signs or overflowing. Together they provide the foundation for the config parser while leaving per-field minimums to task 2.2.

## Approach

The span helper owns whitespace normalization for numeric input and leaves the caller's bytes unchanged. The number parser consumes that view, validates each byte as an ASCII digit, checks the `INT_MAX` bound before each multiply and addition, and assigns the caller's output only after full success. Both are pure core functions exposed through `main.h` and included in the deliverable Makefile; unit and acceptance tests exercise their public behavior.

## Decisions

The approved plan keeps scheduler parsing out of the trim helper because the subject requires the scheduler to match exactly `fifo` or `edf`. Numeric parsing accepts leading and trailing whitespace, but signs, inner whitespace, and non-ASCII digits remain invalid. Field minimums are applied by `parse_config` in task 2.2.3, rather than by this reusable conversion.

## Issues resolved

The phase review found that the minimum-check cross-reference in the 2.1.2 description named the scheduler parser. It was corrected to point to 2.2.3, where config parsing applies the minimums. The roadmap review also rejected case-insensitive or trimmed scheduler input; the approved Phase 02 plan preserves exact `fifo`/`edf` matching, and task 2.1 does not trim scheduler values.

## Issues open

Nothing open.

## Follow-ups

Task 2.2.3 will combine `parse_number` with the field minimum lookup and exact scheduler parser, reporting invalid argument positions as a bit mask. Later runtime work must account for the accepted `INT_MAX` range in time arithmetic and initialization, as noted in the subtask spec.

## Branch notes

The branch contains the approved roadmap and phase plan changes alongside task 2.1. These establish the exact scheduler rule, the phase review corrections, and the shared-file dependency order required for the subsequent parsing subtasks.

## Where to start reviewing

1. [[docs/plan/phase-02-argument-parsing/2.1.1-trim-span.md]] — the span contract and whitespace scope.
2. [[docs/reference/2.1.1-trim-span.md]] — how the first primitive was implemented and tested.
3. [[docs/plan/phase-02-argument-parsing/2.1.2-parse-number.md]] — digit, range, failure-output, and acceptance requirements.
4. [[docs/reference/2.1.2-parse-number.md]] — the conversion approach and its overflow reasoning.
