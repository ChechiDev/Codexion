# Plan review — subtask 2.2.3

**Verdict:** PLAN_APPROVED

## Plan analysis
- Layers: the change only reorganizes declarations in the existing public header; it adds no dependency or behavior.
- Scope: one declared production file (`coders/main.h`) and one cohesive header-layout change. The stable API is preserved, including the span names until their planned replacement.
- Test strategy: four black-box acceptance checks cover section order and placement, stable public declarations, and build/Norm compliance. The current header has no section titles, so AC1 is red before implementation. The harness reported `make check` exit 0 at session start; tests were not run in this sandbox.

## Acceptance tests (traceability)
- AC1 → test_ac1_ordered_sections (red)
- AC2 → test_ac2_declarations_in_sections (red)
- AC3 → test_ac3_stable_public_api (passes against the existing API; protects it during the edit)
- AC4 → test_ac4_header_build_and_norm (build/Norm gate; not run in this sandbox)
