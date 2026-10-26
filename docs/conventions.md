# Conventions (C)

> **Binding subject:** `docs/codexion-rules.md` (42 Codexion). Everything here
> applies only where it does not contradict it; on conflict the subject wins.

Code, names, and comments in **English**. C standard: **C11** (`-std=c11`).

## Toolchain

- Build via the `Makefile` (`make lint`, `make type`, `make test`, `make verify`).
- **`type`** = a strict warning-clean compile: `-Wall -Wextra -Werror -pedantic`.
  A warning is an error. This is the C equivalent of the type gate.
- **`lint`** = `clang-format` (style), `cppcheck` and `clang-tidy` (static
  analysis) when installed. On a bare box they are skipped, and the compiler
  warnings plus the Codex review carry the quality gate.
- Prefer `clang`/`gcc`; keep the code portable between both.

## Layout

- `include/<name>/…h` — **public** headers (the API surface).
- `src/…c` (+ private `.h`) — implementation. One translation unit per module.
- `tests/unit/` — unit tests (Claude). `tests/acceptance/` — oracle (Codex).
- No implementation in public headers except `static inline` where justified.

## Naming

- Functions/variables: `snake_case`. Types/structs: `snake_case_t` (typedef'd).
- Macros and constants: `UPPER_SNAKE`. Prefix public symbols with `<name>_`.
- Names by intent, not by type (`buffer_append`, not `do_data`).

## Errors & resources

- Report failure with an explicit return code (`int`/`enum`) or a sentinel; do
  not hide errors. Use a typed `enum` error set per module.
- **Ownership is explicit**: document who allocates and who frees. Every `malloc`
  has a matching `free` on every path; no leaks (verify with Valgrind/ASan when available).
- Check the return value of every allocation and syscall. No undefined behavior.

## Clean code / SOLID

- One responsibility per function; keep functions short and low-complexity.
  (`clang-tidy` readability checks enforce this when installed; otherwise Codex
  rejects monster functions in review.)
- No mutable global state; pass context via parameters (opaque struct + funcs =
  the C way to inject dependencies). Keep modules decoupled, no circular includes.

## Security (defensive)

- Bounds-check every buffer; prefer `snprintf`/`memcpy_s`-style bounded calls.
  Never `gets`, never unbounded `strcpy`/`sprintf`.
- Treat all external input as untrusted; validate at the boundary.
- No secrets in code; do not log sensitive data.

## Tests

- `tests/unit/` (Claude) and `tests/acceptance/` (Codex, read-only for Claude).
- Default runner: each `tests/**/test_*.c` has its own `main()` using `assert.h`;
  `make test` compiles it against the `src/` objects and runs it (non-zero = fail).
- One test per observable behavior; descriptive names.

## ‹Project additions›

- ‹concurrency rules, allocator, platform specifics…›

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
