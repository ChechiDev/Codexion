# Phase 07 — Coder thread
- goal: the coder routine loops take dongles, compiling, release, debugging, refactoring with correct logs, updates last_compile_start and compile_count under its own mutex, and exits promptly on stop (single coder takes its only dongle and waits to burn out)
