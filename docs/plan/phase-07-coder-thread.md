# Phase 07 — Coder thread
- goal: the coder routine loops take dongles, compiling, release, debugging, refactoring with correct logs, updates last_compile_start and compile_count under its own mutex, and exits promptly on stop (single coder takes its only dongle and waits to burn out)

## 7.1 Coder state
- layer: orchestration
- goal: record a coder's compile start on the simulation clock and count its completed compiles, each under the coder's own mutex and without holding it across a log or a sleep

## 7.2 Coder acquisition
- layer: orchestration
- goal: take the coder's dongles (the pair, or the single dongle when there is one coder) and log `has taken a dongle` once per distinct dongle, releasing them once if the log fails

## 7.3 Coder routine
- layer: orchestration
- goal: the thread routine loops take, compiling, release, debugging, refactoring until stop; one coder takes its only dongle and waits for the stop; any failure releases what is held once, sets stop and wakes every waiter
