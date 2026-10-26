# Implementation 1.2.2 (Rename scheduling policy to mode)

Files touched: `coders/main.h` only.
- `t_policy`/`POLICY_*` -> `t_mode`/`MODE_FIFO`/`MODE_EDF` (`enum e_mode`).
- `t_config.scheduler` -> `t_mode mode;`; tab alignment of all fields redone (`int\t\t`, `t_mode\t`).
- 42 header `Updated:` refreshed from the plugin on a scratch `main.h` (`2026/10/26 18:41:08`); `Created:` kept. No system includes.

## norminette coders/
```
main.c: OK!
main.h: OK!
```

## make verify
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
[OK]    tests passed
```

## make check (tail)
```
[OK]    feature_list.json and the PLAN.md index are in sync with docs/plan/
[OK]    gates consistent (every 'done' has APPROVED)
[OK]    reference docs complete (every 'done' documented)
[OK]    src/ changes are within the in_progress/closing subtask's declared files
[OK]    sealed acceptance tests intact (3 seal(s))
[OK]    approved plan intact (roadmap and phase seals)
[OK]    Harness invariants OK.
```

## CI simulation (clean copy without harness/ and .claude/, CODEXION_TIME_SLACK_MS=40)
```
verify:   [ok] norminette / [skip] cppcheck / [ok] compiles clean / [ok] tests passed
memcheck: [ok] valgrind clean
tsan:     [ok] tsan clean
```
