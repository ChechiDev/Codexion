# Verification (C)

How to prove a subtask works, before declaring `done`.

## The gate: `make verify`

- **lint** — `clang-format`/`cppcheck`/`clang-tidy` when installed (skipped otherwise).
- **type** — strict warning-clean compile (`-Wall -Wextra -Werror -pedantic`).
  Any warning fails the build.
- **test** — every `tests/**/test_*.c` is compiled against the `src/` objects and
  run; a non-zero exit (failed `assert`, crash) fails the gate.

## Acceptance tests (Codex oracle)

- Live in `tests/acceptance/test_<N.M.K>.c`, written by Codex at the plan gate and
  **left red**. They are black-box: they exercise only the public API in
  `include/<name>/`, one test program per subtask, one check per acceptance criterion.

## Recommended extra checks (when the tools exist)

- **Sanitizers**: build tests with `EXTRA_CFLAGS="-fsanitize=address,undefined -g"`
  to catch memory/UB errors.
- **Valgrind**: `valgrind --error-exitcode=1 --leak-check=full ./tests/...bin`.

## ‹Project additions›

- ‹fuzzing, benchmarks, platform matrix…›
