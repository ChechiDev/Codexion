## Overview

Task 3.4 adds serialized subject logging on top of the time and stop-state adapters. It provides bounded number and line formatting, ordinary state output, and a terminal burnout line. All accepted lines share one timestamp and output path, and the burnout operation makes the first stop while keeping later state messages out of the log.

## Approach

The task separates pure formatting from adapter behavior. `build_number_text` and `build_log_line` construct bounded bytes without I/O. `print_log_line` owns elapsed-time lookup and the single unbuffered write, while `print_state` and `print_burnout` own their distinct stop and serialization policies. The shared log mutex keeps each decision ordered with its output; the burnout path takes the stop mutex second, commits the terminal state, and then writes through the common helper.

## Decisions

The approved plans use one unbuffered write for each complete log line, so a successful call makes its output immediately visible to pipe readers. The later 3.4.5 and 3.4.6 subtasks were inserted ahead of burnout so both log operations could use a single shared output implementation.

## Issues resolved

The first burnout implementation duplicated timestamp, formatting, and write logic and used helper verbs outside the project's closed list. The subtask was replanned after the shared logger declaration and extraction; the final implementation was written fresh, calls `print_log_line` once, and uses the allowed `set_stopped_once` helper name.

## Issues open

Nothing open.

## Follow-ups

The monitor's burnout detection cadence and the end-to-end 10 ms deadline remain for later monitor and integrated verification work. User-facing README documentation is planned for phase 09, after the executable is integrated.

## Branch notes


## Where to start reviewing

1. `coders/src/log/print_burnout.c` — see how the terminal stop is claimed and serialized.
2. `coders/src/log/print_log_line.c` — see the shared timestamp, formatting, and write contract.
3. `coders/src/log/print_state.c` — compare the ordinary event policy against the terminal path.
4. `docs/plan/phase-03-time-and-log/3.4.4-print-burnout.md` — review the acceptance criteria and failure contract.
