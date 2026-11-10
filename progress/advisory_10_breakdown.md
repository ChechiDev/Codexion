Advisory only. No files changed or checks/binary runs performed.

1. **The 1 ms offset is compliant and proportionate, but remains a heuristic.** Each coder uses its own seat number, so 4.7 is respected. FIFO still orders actual requests; EDF still orders deadlines, satisfying the arbitration requirements in 5.2.8 and 6.1.5. The burnout clock must remain unchanged. A startup delay also leaves immediate acquisition after refactoring unaffected.

   Your measurements support adopting it as a practical startup improvement. They do **not** establish the general 6.1.6 guarantee: an OS can delay odd threads beyond 1 ms. A deterministic startup protocol would be stronger but adds synchronization and failure handling; I would not require that redesign without further evidence.

   The offset preserves the arbiter’s total-order/deadlock argument. However, the written timing proof needs correction: it assumes requests arrive exactly at `s + C + R`, which real scheduling already violates, and startup requests now have an explicit delay. Account for that delay and state bounded scheduling assumptions. The conservative bound is a sufficient condition, not the definition of all viable parameters. Replace `4 410 200 200 200` as a success example: its minimum repeated cycle is 600 ms.

2. **Revise both specs before sealing.**

   - **10.1.1:** The helper is small and reasonable. Correct the explanation that tickets follow seat order: that is observed behavior, not guaranteed ordering. Add deterministic coverage of stop during the offset and error propagation, preferably with a controlled dependency. Avoid interpreting “at once” as a strict wall-clock upper bound on shared CI.
   - **10.1.2 AC1–AC2:** Useful empirical regressions, but brittle as unconditional CI requirements. `S` added to log upper bounds cannot prevent the program’s real burnout deadline from expiring. Keep the tight scenarios as explicitly empirical checks; use generous parameters for mandatory shared-runner completion checks.
   - **AC3:** Neither the first pair’s identities nor a `<200 ms` gap is guaranteed by the offset. Three attempts reduce failure frequency without making the criterion sound. Adding slack to permit a gap above 200 ms would cease to demonstrate overlap. Test the offset invocation deterministically and assess actual parallelism separately under controlled conditions.
   - **AC4–AC5:** Sensible. Add explicit offset-error routing through `failed`/stop cleanup. A `310 + S` upper bound is a CI tolerance, not evidence that the subject’s strict 10 ms requirement was met.
   - **Norm:** The proposed loop rewrite fits exactly 25 interior lines if implemented literally; the file retains five functions. There is no spare line budget. The separate helper avoids a sixth function in this file.
   - **Sealed oracles:** I find **no assertion necessarily broken by the offset**. They check cycles, progress, cleanup and stop latency rather than immediate even-seat acquisition:
     - [test_7.3.1.c:477](/home/sergi/dev/codexion/tests/acceptance/test_7.3.1.c:477) allows `100 + slack` for stop; [line 513](/home/sergi/dev/codexion/tests/acceptance/test_7.3.1.c:513) requires the interrupted even coder’s count to remain zero.
     - [test_9.2.2.c:235](/home/sergi/dev/codexion/tests/acceptance/test_9.2.2.c:235) starts the general progress scenario; the directly injected failures use coder 1.
     - [test_ISS-32.1.c:306](/home/sergi/dev/codexion/tests/acceptance/test_ISS-32.1.c:306) starts all coders, then checks stop latency at line 320 and aggregate progress at line 329.
     - [test_ISS-34.1.c:414](/home/sergi/dev/codexion/tests/acceptance/test_ISS-34.1.c:414) similarly checks latency at line 427 and progress at line 429.

   Do not pre-authorize weakening those oracles if a later run fails; diagnose the actual assertion first.

3. **Recommend option A: three subtasks.** Viability, observable rules, and evaluation/tooling scenarios have distinct evidence and failure modes. Declare `.github/workflows/ci.yml` where `SCENARIOS` changes require it. FIFO/EDF request order is not generally inferable from compilation logs alone: controlled request admission needs existing API-level oracles or an explicit test fixture. Likewise, cooldown assertions must account for the fact that release itself is not logged.

4. **The harness mechanically permits green plan-gate oracles; the written workflow does not currently authorize this test-only case.** [review_with_codex.sh:193](/home/sergi/dev/codexion/harness/review_with_codex.sh:193) supplies the pre-review check result; its sealing path uses the approval verdict, without requiring a red-test result. [_guard.py:759](/home/sergi/dev/codexion/harness/_guard.py:759) checks changed production paths against declared scope; it does not require production changes or a red-to-green transition.

   Declare these honestly as **“Test-only regression/verification; no production behavior change; acceptance oracles may already pass at the plan gate.”** List their actual test/tooling files and objective verification. Do **not** call them structural refactors. Before gating, explicitly extend the workflow exception in AGENTS.md and reconcile the reviewer’s “leave red” instruction; spec wording alone should not silently override that policy. Do not manufacture a failing assertion merely to satisfy the red requirement.

Recommendation: PROCEED_WITH_CHANGES