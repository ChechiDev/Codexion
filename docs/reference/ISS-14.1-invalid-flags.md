---
id: ISS-14.1
alias:
  - Invalid argument flags
tags:
  - codexion
  - parsing
  - validation
  - refactor
date: 2026-11-02 10:12
---

<!-- section: concept -->
## Report invalid arguments with indexed flags

`parse_config` validates eight command-line values: seven numeric fields and
the scheduler. A bit mask can encode the invalid positions, but the caller has
to interpret bits to find the corresponding argument. The revised contract
uses an integer array whose indices directly match those positions:

```c
int invalid[9];
int count;

count = parse_config(argv, &config, invalid);
```

Index zero is reserved. On every call, the parser writes `0` or `1` to indices
1 through 8 and returns the number of invalid arguments. For example, if
positions 2, 3, and 8 fail:

| Index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| `invalid` | caller value | 0 | 1 | 1 | 0 | 0 | 0 | 0 | 1 |

The return value is `3`. Index zero is never written, and stale caller values
at indices 1 through 8 are replaced. This makes the array safe to reuse without
clearing it first. The caller can report all errors in argument order by
scanning it directly:

```text
for position 1 through 8:
    if invalid[position] is set, print that position's diagnostic
```

The parser still validates every field, uses the existing numeric and mode
parsers, and prints nothing. If the returned count is nonzero, configuration
contents are unspecified. The entry point owns the stack array and existing
CLI functions continue to own user-facing messages.

<!-- section: planning -->
## How it was planned

The issue targets the bit-mask contract introduced by task 2.3.3. The approved
subtask declares `coders/src/parsing/parse_config.c`, `coders/main.h`, and
`coders/main.c`: the function contract, its public prototype, and its only
caller form one buildable change. Acceptance covers valid configuration,
flag initialization and counts, every invalid position, preservation of index
zero and input strings, unchanged binary diagnostics, and build, Norm, and
memory checks. The sealed `tests/acceptance/test_2.3.3.c` is migrated to the
new API; `tests/acceptance/test_2.4.2.c` remains unchanged as regression
coverage.

<!-- section: execution -->
## How it was executed

The parser keeps the existing field-validation path: numeric arguments go
through `parse_field`, and the scheduler goes through `parse_mode`. It records
each outcome in the matching array slot and increments a count for each
failure; it does not stop early or duplicate validation rules. `main` passes a
stack `int invalid[9]` and scans positions in ascending order before printing
usage once on an invalid run. This changes the public C API while preserving
the binary's error ordering and output contract.

<!-- section: problems -->
## Problems & resolution

The initial issue review asked to split the parser and caller subtasks, move
source inspection out of acceptance criteria, repair a delimiter-sensitive
criterion, and correct the structural-refactor classification. The approved
plan kept the API and sole caller together because either side alone would not
build, moved source-only requirements into `Code review check:`, and retained
observable API and binary criteria. No validation rule or user-facing output
changed.

<!-- section: files -->
## Files created / modified

- [[coders/src/parsing/parse_config.c]] — fills one flag per argument and returns the invalid count.
- [[coders/main.h]] — declares the new `parse_config` signature.
- [[coders/main.c]] — supplies stack storage and reports flagged errors in order.
- [[tests/acceptance/test_ISS-14.1.c]] — covers the new parser API and binary behavior.
- [[tests/acceptance/test_2.3.3.c]] — migrates sealed assertions from mask values to flags and counts.
- [[docs/plan/issues/ISS-14-refactor-replace-parse-config-bit-mask-with-invalid-flags-2-3/ISS-14.1-invalid-flags.md]] — records the approved scope and done status.
- [[docs/reference/2.3.3-parse-config.md]] — updates the earlier parser reference to the flag contract.
- [[docs/reference/2.4.1-print-arg-error.md]] — updates how parser results are described.
- [[docs/reference/2.4.2-validate-arguments.md]] — updates the caller's validation flow.
- [[progress/pr_ISS-14.md]] — supplies the task pull request narrative.
- [[progress/close_ISS-14.1.msg]] — supplies the close commit message.
