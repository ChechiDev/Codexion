# Phase 05 — Simulation state init and teardown
- goal: build the `t_sim` (coders, dongles with mutex/cond/heap, stop and log mutexes, seat-to-dongle mapping with 1 dongle for 1 coder) and tear it down completely, cleaning up partial initialisation on any malloc or pthread init failure without leaks
