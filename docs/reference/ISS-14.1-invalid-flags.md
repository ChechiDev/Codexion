---
id: ISS-14.1
alias:
  - Invalid argument flags
tags:
  - codexion
  - parsing
  - validation
  - refactor
date: 2026-11-02 12:00
---

<!-- section: concept -->
## Report invalid arguments with indexed flags

`parse_config` validates eight command-line values: seven numeric fields and
the scheduler. A bit mask can encode which positions failed, but it requires
the caller to decode bits. The updated contract uses an array whose indices
match the argument positions:

```c
int invalid[9];
int count;

count = parse_config(argv, &config, invalid);
```

Index 0 is reserved. For every call, the parser writes `0` or `1` to positions
1 through 8 and returns the number of invalid arguments. For example, if
positions 2, 3, and 8 fail:

| Index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| `invalid` | caller value | 0 | 1 | 1 | 0 | 0 | 0 | 0 | 1 |

The return value is `3`. Index zero is never written, and stale values at
indices 1 through 8 are replaced, so callers can reuse the array without
clearing it. The entry point scans the flags in ascending order and delegates
each diagnostic to the CLI layer:

```text
invalid[2] = 1  -> report argument 2
invalid[3] = 1  -> report argument 3
invalid[8] = 1  -> report argument 8
```

The parser still checks every field, uses the existing numeric and scheduler
parsers, and prints nothing. If the count is nonzero, configuration contents
are unspecified and must not be used. The entry point owns the stack array;
the CLI continues to own user-facing messages.

<!-- section: planning -->
## How it was planned

The issue replaces the bit-mask result from task 2.3.3 with a per-position
array and invalid count. The approved subtask declares
[[coders/src/parsing/parse_config.c]], [[coders/main.h]], and [[coders/main.c]]:
the parser contract, its public prototype, and its only caller form one
buildable change. Acceptance covers valid input, flag initialization and
counts, every invalid position, preservation of index zero and input strings,
unchanged binary diagnostics, and build, Norm, and memory checks. The sealed
[[tests/acceptance/test_2.3.3.c]] is migrated to the new API;
[[tests/acceptance/test_2.4.2.c]] remains unchanged as regression coverage.

<!-- section: execution -->
## How it was executed

The parser retains the existing validation path: positions 1 through 7 go
through `parse_field`, and position 8 goes through `parse_mode`. It writes each
result into the matching flag slot and counts failures without stopping early
or duplicating validation rules. `main` passes stack storage and scans positions
in order, printing each existing diagnostic and then usage once. The public C
API changes while the program's user-visible error order and output contract
stay the same.

<!-- section: problems -->
## Problems & resolution

The first issue review requested splitting the parser and caller work, moving
source-only checks out of acceptance criteria, fixing a delimiter-sensitive
criterion, and correcting the structural-refactor classification. The final
approved plan keeps the API and its only caller together because changing the
prototype alone would leave the program uncompilable. It moves source
constraints into `Code review check:` and keeps observable API and binary
behavior in acceptance. No validation rule or user-facing output changed.

<!-- section: files -->
## Files created / modified

- [[coders/src/parsing/parse_config.c]] — fills per-position flags and returns the invalid count.
- [[coders/main.h]] — declares the parser's flag-array API.
- [[coders/main.c]] — owns the stack array and reports flagged positions in order.
- [[tests/unit/test_parse_config.c]] — exercises valid input, counts, flag reset, and index-zero preservation.
- [[tests/acceptance/test_2.3.3.c]] — migrated mask assertions to flag and count assertions.
- [[tests/acceptance/test_ISS-14.1.c]] — covers the new API and unchanged binary diagnostics.
- [[docs/plan/issues/ISS-14-refactor-replace-parse-config-bit-mask-with-invalid-flags-2-3/ISS-14.1-invalid-flags.md]] — records the approved scope and completion status.
- [[docs/reference/2.3.3-parse-config.md]] — updates the parser concept to describe flags and a count.
- [[docs/reference/2.4.1-print-arg-error.md]] — describes position flags as the CLI's diagnostic input.
- [[docs/reference/2.4.2-validate-arguments.md]] — documents the entry point's ordered flag scan.
