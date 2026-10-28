# Phase 03 — Time and serialized logging
- goal: millisecond clock relative to the simulation start (gettimeofday), a stop-aware sliced sleep, and a mutex-serialized logger that prints the five subject state lines and nothing after the stop except the burnout line

## 3.1 Clock
- layer: adapters
- goal: millisecond time from `gettimeofday`, absolute and relative to the simulation start

## 3.2 Stop state
- layer: adapters
- goal: a first minimal `t_sim` (start time, stop flag, stop and log mutexes) with the mutex-guarded stop flag reader and writer; phase 05 extends it and owns its init and teardown

## 3.3 Sliced sleep
- layer: adapters
- goal: sleep a duration in short slices that return early once the simulation stops

## 3.4 Serialized logger
- layer: adapters
- goal: build each subject log line in a local buffer and send it with one unbuffered `write` under one log mutex; nothing after the stop, and the single burnout line only when the burnout makes the first stop
