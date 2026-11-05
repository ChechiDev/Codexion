# Phase 08 — Monitor thread
- goal: a separate monitor thread that detects a burnout and logs it within 10 ms, or detects that every coder reached number_of_compiles_required, then sets stop and wakes all waiting threads

## 8.1 Monitor checks
- layer: orchestration
- goal: tell whether one coder has burned out (clock and `last_compile_start` read under the coder's own mutex, `>=` deadline) and whether every coder reached `number_of_compiles_required` (0 required is met at once), without logging and with outputs untouched on any failure

## 8.2 Monitor routine
- layer: orchestration
- goal: the monitor thread scans about every 1 ms; on the first burnout it prints it at once through `print_burnout` and wakes every waiter; when the compile target is met (0 required wins) it sets stop and wakes every waiter; it leaves silently when someone else stopped, and any failure sets stop and wakes every waiter
