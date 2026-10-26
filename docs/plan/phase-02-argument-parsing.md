# Phase 02 — Argument parsing and validation
- goal: strictly parse and validate the 8 arguments into a config (digits only, overflow-safe up to INT_MAX, per-field minimums, scheduler exactly fifo/edf); any invalid input prints usage to stderr and exits non-zero before allocating or starting threads
