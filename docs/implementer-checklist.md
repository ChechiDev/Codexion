# Implementer checklist

> Passed to every implementer prompt. Lessons from earlier tasks (mainly task
> 6.3), reviewed by Codex (`progress/advisory_implementer_checklist.md`).
> `docs/codexion-rules.md` and `docs/conventions.md` win on conflict.

## Before coding

- [ ] Read the subtask spec, its `files:` list and the sealed acceptance test.
- [ ] Grep `tests/` and `coders/main.h` for every new name before adding it
      (name clashes with sealed acceptance helpers already happened).

## Naming, Norm and files

- [ ] Every function name, **including `static` helpers**, starts with a verb
      from the closed list in `docs/conventions.md` (only the fixed-name
      exceptions listed there, such as `main`, are exempt). A missing verb is
      added on a `style/<name>` branch and used only once merged; never invent one.
- [ ] Functions of at most 25 lines and 4 parameters, at most 5 functions per
      file, line length within the Norm. Run `norminette coders/` before reporting.
- [ ] Update the declared `coders/Makefile` and `main.h` entries, and refresh
      the 42 header of every touched file.

## Ownership and failure paths

- [ ] On **every return** (success and each failure), the locks held match
      what the spec promises: most functions return holding nothing, but some
      succeed holding mutexes (e.g. `wait_for_dongles` returns holding both).
- [ ] Check **every fallible call**, cleanup unlocks included; after one
      cleanup failure, keep doing the remaining cleanup.
- [ ] Track ownership across helper calls so nothing is unlocked twice.
- [ ] Follow the spec's fatal path; do not invent your own cleanup (queued
      requests cannot be removed at random: acquisition releases its mutexes,
      calls `set_stopped` and `wake_dongles`).
- [ ] Leave out-parameters unchanged on failure.
- [ ] Keep the agreed lock order (lower dongle index first); never lock the
      same mutex twice.
- [ ] Same pointer (one coder, one dongle): one lock, one push/pop and one
      state update.

## Verification

- [ ] Run `make check` sequentially, in the **foreground**, never in the
      background and never alongside another `make` (acceptance tests run
      `make -C coders`). Trust it only when it finished and exited 0.
- [ ] Run `make tsan` and `make memcheck`. They cover unit tests and binary
      scenarios only: also build and run the threaded acceptance tests under
      TSan, and those that allocate under valgrind.
- [ ] `make tsan` already applies `setarch -R`; do the same for manual TSan
      runs. Exit 66 is not proof either way: a "unexpected memory mapping"
      startup failure is a failed run that needs a working rerun, not a pass.
- [ ] If a diagnostic points at the sealed oracle (e.g. a data race inside the
      test, as in 6.3.4), stop and report it for correction and re-sealing;
      never edit `tests/acceptance/`.
- [ ] In `progress/impl_<id>.md` report the exact commands, their exit codes,
      the relevant diagnostics, and every check you skipped.
