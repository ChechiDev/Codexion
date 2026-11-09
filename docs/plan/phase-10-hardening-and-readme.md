# Phase 10 — Hardening and README
- goal: make non-neighbouring coders start in parallel with a one-time offset, then prove the whole program against the subject with full-binary simulation tests (log rules, exclusion, fairness, cooldown; exact fifo/edf request order stays proven by the sealed phase 06 API oracles; no burnout under edf at and above the PLAN.md viability boundary including an N = 6 case, typical evaluation runs, valgrind leak-free, TSan race-free, Norm-clean) and complete the README with its mandatory sections, real outputs and didactic tone

## 10.1 Startup stagger
- layer: orchestration
- goal: let non-neighbouring coders compile in parallel from the start by delaying even seats once, without changing the fifo/edf arbitration

## 10.2 Subject verification
- layer: orchestration
- goal: prove the full binary against the subject with test-only oracles: viability boundary, observable rules and typical evaluation runs

## 10.3 README
- layer: interfaces
- goal: complete the README with the mandatory sections of subject section 7, real outputs and didactic tone
