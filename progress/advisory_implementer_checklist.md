# docs/implementer-checklist.md — Codex advisory review (2026-11-04)

Advisory only (no gate, no seal). All points applied.

1. Wrong: "unlock exactly once" ignores functions that return holding locks (wait_for_dongles) -> "ownership on each return matches the spec". "Release queued requests" contradicts the 6.3.4 fatal path (release mutexes, set_stopped, wake_dongles). Verb rule must mention the fixed-name exceptions (main).
2. Missing: check every fallible call incl. cleanup unlocks and keep cleaning up; track ownership across helpers; TSan on threaded acceptance tests (make tsan covers unit + binary scenarios only); report oracle defects for re-seal; keep out-parameters on failure; update Makefile/main.h entries and 42 headers.
3. Merge: binary-timestamp check into "sequential make check that exited 0"; single same-pointer invariant.
4. Wording: style/<name> branch, use once merged; make tsan already applies setarch -R, exit 66 alone inconclusive; define "real outputs" (commands, exit codes, diagnostics, skipped checks).
