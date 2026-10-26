# Conventions (C)

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
