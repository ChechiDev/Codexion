# Phase 05 — Simulation state init and teardown
- goal: build the `t_sim` (coders, dongles with mutex/cond/heap, stop and log mutexes, seat-to-dongle mapping with 1 dongle for 1 coder) and tear it down completely, cleaning up partial initialisation on any malloc or pthread init failure without leaks

## 5.1 Seat mapping
- layer: core
- goal: map each 0-based seat to its two dongle indices, lower index first, with a single shared dongle when there is one coder and no overflow at the int bounds

## 5.2 Dongles
- layer: adapters
- goal: initialise one dongle (mutex, cond, capacity-2 request heap, available, never released) and the array of N dongles, rolling back exactly what succeeded on any failure, and tear them down in reverse

## 5.3 Coders
- layer: adapters
- goal: initialise one coder (id, ordered dongle pair, own mutex, zeroed compile state, back pointer) and the array of N coders with the same rollback guarantees, and tear them down in reverse

## 5.4 Simulation
- layer: adapters
- goal: build the whole `t_sim` (config, stop/log/ticket mutexes, dongles, coders) in a fixed order with a full reverse rollback on any failure, and free all of it; threads are not started here (phase 09)
