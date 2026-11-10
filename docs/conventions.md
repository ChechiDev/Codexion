# Conventions (C)

> **Binding subject:** `docs/codexion-rules.md` (42 Codexion). Everything here
> applies only where it does not contradict it; on conflict the subject wins.
> The Norm (norminette) is part of the subject.

Code, names, and comments in **English**.

## Toolchain

- Build via the root `Makefile` (`make lint`, `make type`, `make test`, `make verify`).
- **`lint`** = `norminette coders/` (any error is a fail), plus `cppcheck` when
  installed.
- **`type`** = a warning-clean compile of the deliverable with
  `cc -Wall -Wextra -Werror -pthread`, plus the no-relink check. A warning is an
  error.

## Layout

```
coders/
├── Makefile           # deliverable build (NAME, all, clean, fclean, re)
├── main.c             # entry point
├── main.h             # the single header of the project
└── src/
    ├── utils/         # generic helpers, no domain knowledge
    └── <module>/      # one folder per module: parsing/, cli/, ...
```

- **One header only**: `coders/main.h`. No `include/` folder, no private headers.
- **One public function per `.c` file**, and the file is named after it
  (`is_space` lives in `src/utils/is_space.c`). A file may add `static` helpers
  that only make sense inside it (the Norm caps a file at 5 functions).
- `tests/unit/` — unit tests (Claude). `tests/acceptance/` — oracle (Codex).

## `main.h` layout

Ordered sections, each opened by a one-line title comment in this exact format:

```c
/* ---- Macros ---- */
/* ---- Types ---- */
/* ---- Utils ---- */
/* ---- Parsing ---- */
/* ---- CLI ---- */
```

- **Macros**, then **Types** (every `typedef`, since a type can be shared by
  several modules), then **one prototype section per `src/` folder**, `Utils`
  first. The title is the folder name capitalised (`src/parsing/` → `Parsing`);
  `CLI` stays upper case. Prototypes of `main.c` itself, if any, go last under
  `/* ---- Main ---- */`.
- A new `src/` folder adds its section; an empty section is not written.
- `main.h` includes only the system headers its own types need: `<pthread.h>`
  (for `pthread_mutex_t`, `pthread_cond_t`, `pthread_t` inside the structs).
  Everything a function uses (`<stdio.h>`, `<sys/time.h>`, `<unistd.h>`, ...)
  is included in its `.c`, not in `main.h`.

## Utils

`coders/src/utils/` holds helpers that are:

- **Generic**: they know nothing about the domain (no coders, dongles, monitor,
  `t_config`). If it needs a domain type, it belongs to its module.
- **Reusable**: used, or obviously usable, by more than one function.

Rules:

- When a function can be written more cleanly with a utils helper, use or add
  the helper instead of inlining the logic (e.g. `is_digit(c)` rather than
  `c >= '0' && c <= '9'`).
- **Never reimplement an authorized function** (`strlen`, `strcmp`, `memset`,
  ...): call the original.

## Naming

- Functions and variables: `snake_case`. Types follow the Norm: `s_` struct,
  `t_` typedef, `e_` enum, `u_` union. Macros and enum values: `UPPER_SNAKE`.
- Names by intent, not by type.
- **Every function name starts with a verb from this closed list** (public and
  `static` alike):

  | Prefix | Use | Example |
  |---|---|---|
  | `is_` / `has_` | yes/no question, returns 0 or 1 | `is_space`, `has_finished` |
  | `get_` | reads or computes a value, changes nothing | `get_arg_minimum` |
  | `set_` | changes a field or a state | `set_mode` |
  | `build_` / `init_` | builds or initialises a structure | `init_config` |
  | `free_` / `destroy_` | releases resources | `destroy_mutexes` |
  | `parse_` | turns text into a validated value | `parse_number` |
  | `print_` | writes to an output | `print_usage` |
  | `trim_` | trims text | `trim_slice` |
  | `sleep_` | waits for a duration | `sleep_ms` |
  | `wait_` | blocks until a condition holds | `wait_for_dongles` |
  | `wake_` | wakes the threads blocked on a condition | `wake_dongles` |
  | `take_` / `release_` | acquires / gives back a shared resource | `take_dongles`, `release_dongles` |
  | `push_` / `pop_` | adds to / removes from a queue | `push_request`, `pop_request` |
  | `sift_` | restores the heap order after a change | `sift_up` |
  | `start_` / `join_` | launches / waits for threads | `start_threads`, `join_threads` |
  | `run_` | body of a thread routine; also the one whole-run driver `run_simulation` (init → threads → join → teardown) | `run_coder`, `run_monitor`, `run_simulation` |

  A new verb is added to this table first (in a `style/` branch), then used.
- **Exception — names fixed by the subject keep their exact spelling**: `main`,
  the authorized functions, the binary `codexion`, the argument names
  (`number_of_coders`, ..., `scheduler`; they are also the `t_config` field
  names, except `scheduler` which maps to `mode`) and the log messages.

## Errors & resources

- Report failure with an explicit return code (`0` ok / `-1` error, or an
  `enum`); do not hide errors. Results go through an out-parameter, which is
  left untouched on failure.
- **Ownership is explicit**: document who allocates and who frees. Every `malloc`
  has a matching `free` on every path; no leaks (`make memcheck`).
- Check the return value of every allocation and syscall. No undefined behavior.

## Clean code

- One responsibility per function; keep functions short (the Norm caps them at
  25 lines).
- No global variables (subject); pass context via parameters.

## Security (defensive)

- Bounds-check every buffer; prefer views (`t_slice`) over copies.
- Treat all external input (`argv`) as untrusted; validate at the boundary.

## Tests

- `tests/unit/` (Claude) and `tests/acceptance/` (Codex, read-only for Claude).
- Default runner: each `tests/**/test_*.c` has its own `main()` using `assert.h`;
  `make test` compiles it against the `coders/` objects and runs it (non-zero = fail).
- One test per observable behavior; descriptive names.

## Style branches

Project-wide convention changes are made on a `style/<name>` branch: first the
rule here, then the refactor subtasks that apply it to the existing code. Once
merged, the rule binds every later subtask. Superseded subtasks stay as
delivered (commit, spec and reference doc unchanged); the new subtask states
what it supersedes.

Changing an approved plan breaks its seal, so `make check` goes red and the
reviewer will not approve. Re-gate it through the project wrapper, which counts
the check as green only when every failure is a plan seal under re-review
(roadmap first, then each changed phase):

```bash
HARNESS_CHECK_CMD="./.claude/scripts/regate_check.sh roadmap" ./harness/review_with_codex.sh roadmap
HARNESS_CHECK_CMD="./.claude/scripts/regate_check.sh phase 02" ./harness/review_with_codex.sh phase 02
```

`harness/` and `.claude/{agents,codex,skills}` are symlinks to the shared global
harness: never edit through them. Project-level harness tooling goes in
`.claude/scripts/` (a real folder, kept out of the PR file list); rules go in
`docs/`.

## Project additions

### 42 header (every file in `coders/`)

Every `.c`, `.h` and the `Makefile` in `coders/` start with the official 42
header (norminette fails with `INVALID_HEADER` without it), followed by one
empty line. **Never write it by hand: generate it with the official `:Stdheader`
plugin** (`42Paris/42header`, installed at `~/.vim/pack/42/start/42header`;
login `sperez-l`, mail `sperez-l@student.42barcelona.com`).

The harness blocks shell writes to `coders/`, so generate the header on a
scratch copy with the same file name and paste it with the Edit/Write tools:

```bash
d="$(mktemp -d)" && : > "$d/main.c" && \
vim -u NONE -N -es --cmd 'set rtp+=~/.vim/pack/42/start/42header' \
  -c "let g:user42='sperez-l'" -c "let g:mail42='sperez-l@student.42barcelona.com'" \
  -c 'runtime! plugin/stdheader.vim' -c 'Stdheader' -c 'wq' "$d/main.c" && cat "$d/main.c"
```

Expected shape (the mail is too long for `By: login <mail>`, so the plugin
writes the mail alone; `Created`/`Updated` carry the real date):

```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 16:40:33 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/26 16:40:33 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
```

`Makefile` gets the `#` variant (`# ****...**** #`) from the same command run on
a file named `Makefile`. When editing an existing file, refresh its `Updated:`
line the same way (the plugin also does it on every `:w` in Vim).

**Legacy exception (user decision, 2026-11-09).** Some headers committed up to
the end of phase 09 (`main` at `f4b8c33`) may have been written by hand: 36 of
the 61 files have a `Created` or `Updated` time with seconds `:00`, a strong but
unproven sign. Those headers are accepted as they are, and their timestamps are
not reliable evidence of when a file was created or changed. They are not
regenerated solely because their provenance is uncertain: regeneration would
record the regeneration date, not establish the original file creation date.
The exception covers only those existing headers. Any new file gets a
generated header, and any later edit to a legacy file refreshes its `Updated:`
line with the plugin. Analysis: `progress/advisory_stdheader_debt.md`.

### Build and test tooling (root Makefile, not delivered)

- Production code lives in `coders/` (`harness.toml` `src_dirs = ["coders"]`); the
  deliverable `coders/Makefile` is the only thing that builds `codexion`.
- Tests link `build/obj/coders/**/*.o` (every source except `coders/main.c`) with
  flags `-Wall -Wextra -Werror -pthread -Icoders` and include `"main.h"`.
- Simulation tests run the binary from the `CODEXION_BIN` env var (set by
  `make test`; default `coders/codexion`) and widen every timing assertion by
  `CODEXION_TIME_SLACK_MS` (default 0 locally, 40 in CI). The subject limits
  themselves (burnout logged within 10 ms) are never relaxed locally.
- `make verify` = norminette + build + no-relink check + tests.
  `make memcheck` = valgrind on `tests/unit/` and on the `SCENARIOS` runs.
  `make tsan` = `-fsanitize=thread` build of the unit tests and of the binary,
  run on the same scenarios. CI runs all three on every push.

### README.md (repo root, written in English)

Must satisfy `docs/codexion-rules.md` §7 and these style rules:

- **No icons or emojis** anywhere (titles included).
- First line, in italics:
  `*This project has been created as part of the 42 curriculum by sperez-l.*`
- Mandatory sections: **Description**, **Instructions**, **Resources** (classic
  references + how AI was used, for which tasks and in which parts),
  **Blocking cases handled**, **Thread synchronization mechanisms** (with examples
  of how races are avoided and how coders and monitor communicate thread-safely).
- **Tone: didactic and conversational.** Explain as if to a peer who has never
  seen the project: short paragraphs, plain words, the "why" before the "how".
- **Instructions** covers the most relevant parts of the project with
  explanatory examples: how to build, the 8 arguments one by one, several
  example runs (normal end by compile count, a burnout, `fifo` vs `edf`, invalid
  input) each with its **real output** and a short walkthrough of what it shows.
- Grows with the project: every subtask with user-visible behaviour updates it
  at close; examples must match the current binary's actual output.
