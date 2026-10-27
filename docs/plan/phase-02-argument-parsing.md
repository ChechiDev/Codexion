# Phase 02 — Argument parsing and validation
- goal: parse and validate the 8 arguments into a config with reusable primitives (one shared trim of surrounding whitespace for numbers, digits only, overflow-safe up to INT_MAX, per-field minimums, scheduler exactly fifo/edf); any invalid input prints a specific error and the usage to stderr and exits non-zero before allocating or starting threads

## 2.1 Parsing primitives
- layer: core
- goal: reusable building blocks for the numeric arguments: a whitespace trim that returns a span without copying, and a digits-only, overflow-safe string to `int` conversion that reports failure instead of guessing

## 2.2 Apply project conventions
- layer: core
- goal: bring the delivered code in line with `docs/conventions.md` (branch `style/project-conventions`): user-facing messages in `cli/`, `main.h` split into titled sections, `t_span` renamed to `t_slice`, and generic helpers (`is_space`, `is_digit`, `trim_slice`) in `src/utils/` reused by the parser, with no behaviour change

## 2.3 Config parser
- layer: core
- goal: turn `argv[1..8]` into a `t_config`, applying each field's minimum and the exact `fifo`/`edf` rule, and report every invalid argument as a bit mask of positions 1..8

## 2.4 Entry point validation
- layer: interfaces
- goal: `main` rejects any invalid argument with a specific error line plus the usage on stderr and exit status 1, before allocating or starting anything
