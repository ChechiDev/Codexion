# 9.5.x specs and gate decision — Codex advisory (2026-11-07)

Advisory only.

1. **El gate ya revisa ambas specs con el resto de la fase**: cobertura, alcance, dependencias y criterios observables; puede pedir cambios antes de aprobar. La aprobación sella el plan, pero un rechazo permite corregirlo y repetir el gate ([reviewer.md:94](/home/sergi/dev/codexion/.claude/codex/reviewer.md:94), [leader.md:73](/home/sergi/dev/codexion/.claude/agents/leader.md:73)).

2. **README presenta una contradicción documental, no un fallo inequívoco de `files:`.** [PLAN.md:234](/home/sergi/dev/codexion/docs/plan/PLAN.md:234) limita la lista a producción bajo `src/` o `include/`, pero la regla específica del proyecto exige declarar README y encarga su actualización a Codex al cierre ([AGENTS.md:71](/home/sergi/dev/codexion/AGENTS.md:71)). Por tanto, no lo quitaría automáticamente de 9.5.2. Sí aclararía AC8: exigir README actualizado en aceptación puede bloquear el code gate antes del cierre ([9.5.2:17](/home/sergi/dev/codexion/docs/plan/phase-09-lifecycle/9.5.2-main-runs-simulation.md:17)).

3. **9.5.1 tiene una contradicción textual clara:** el encabezado “Not printed after an output failure on stdout only” dice lo contrario del párrafo y de AC5 de 9.5.2: precisamente debe imprimirse por stderr cuando falla stdout ([9.5.1:31](/home/sergi/dev/codexion/docs/plan/phase-09-lifecycle/9.5.1-print-run-error.md:31)). Sus AC son observables, aunque AC3 repite propiedades ya garantizadas por AC1.

4. **Los estados de salida y el contrato principal encajan:** 0 para objetivo, burnout o cero compilaciones; 1 para argumentos; 2 para error interno. También se respeta la prohibición de reutilizar o limpiar `sim` tras el retorno ([9.5.2:23](/home/sergi/dev/codexion/docs/plan/phase-09-lifecycle/9.5.2-main-runs-simulation.md:23)). Quedan referencias desactualizadas en 9.4.1: atribuye a 9.5.1 la integración de `main`, que ahora corresponde a 9.5.2 ([9.4.1:29](/home/sergi/dev/codexion/docs/plan/phase-09-lifecycle/9.4.1-run-simulation.md:29)).

5. **Los AC de integración son razonables**, con watchdog, captura de salidas y `/dev/full`. Revisaría la fragilidad del límite absoluto de 100 ms y completaría AC3 con stderr vacío y timestamps no decrecientes, como ya exige 9.4.1 ([9.5.2:12](/home/sergi/dev/codexion/docs/plan/phase-09-lifecycle/9.5.2-main-runs-simulation.md:12), [9.4.1:13](/home/sergi/dev/codexion/docs/plan/phase-09-lifecycle/9.4.1-run-simulation.md:13)).

6. Una asesoría separada podría ahorrar una repetición del gate, pero duplicaría buena parte de su revisión obligatoria. Esta lectura ya identifica los ajustes evidentes; conviene resolverlos y dejar la evaluación conjunta al gate. **Recomendación: (b), ir al gate de fase tras esos ajustes, sin otra asesoría independiente.**