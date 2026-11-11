# Implementation 10.3.1 Complete README

Files touched: `README.md` only.

Changes: removed stale skeleton text and placeholders; moved the argument
validation paragraph and example into Instructions; added example runs (normal
end, burnout, fifo/edf), the viability bound and the START_TURN_MS offset;
filled Blocking cases handled and Thread synchronization mechanisms; added the
AI paragraph verbatim in Resources. All shown outputs were copied from real
runs of `./coders/codexion`. Usage blocks show the binary's fixed text
(`./codexion ...`), as AC6 requires.

## make verify (real output, summarized by grep of the final run)

```
[OK]    norminette
[SKIP]  cppcheck not installed
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
[OK]    tests passed
```

Note: the very first `make verify` run, made right after a fresh build,
printed `[FAIL] coders/Makefile relinks` once (the root build had just
relinked); the immediate re-run was fully green, including
tests/acceptance/test_10.3.1.c.

## Code gate r1 fixes (README.md only)

- Deadlock/Coffman: now distinguishes the two mechanisms (hold and wait removed
  by taking both dongles atomically only when both are ready; circular wait
  removed by locking dongle mutexes in lower-index order); mutual exclusion and
  no preemption stay.
- Burnout: "as soon as time_to_burnout ms have elapsed" (matches
  get_burned_out.c `last <= now - time_to_burnout`); 400 ms example reworded.

make verify after the fixes (first run hit an unrelated timing red in
tests/acceptance/test_2.4.1.c, WSL2 clock jump; immediate re-run green):

```
[OK]    norminette
[SKIP]  cppcheck not installed
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
[OK]    tests passed
```
