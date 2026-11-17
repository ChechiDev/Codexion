---
aliases:
  - Live coding EDF
  - Desempate por id más alto
tags:
  - codexion
  - 42
  - defensa
  - live-coding
author: sperez-l
---

# Live coding: en EDF, si hay empate, gana el id más alto

> [!abstract] Qué es este documento
> Es mi guion para el live coding de la defensa. Aquí está qué me piden, qué
> archivo toco, el código de antes y de después, y el porqué de cada línea.
> Al final hay una checklist para seguirla delante del evaluador sin
> saltarme nada.

## 1. Qué me piden

En modo `edf` el scheduler ya da los dongles al coder que **antes va a llegar
al burnout** (el deadline más pequeño). Eso no cambia.

Lo que cambia es **qué pasa cuando dos coders empatan**, es decir, cuando los
dos tienen exactamente el mismo deadline. Ahora mismo gana el que pidió
primero (el que tiene el `ticket` más bajo). Después del cambio tiene que
ganar **el coder con el id más alto**.

Dicho corto:

| | Primer criterio | Si empatan |
|---|---|---|
| Antes | deadline más pequeño | ticket más bajo (quien pidió antes) |
| Después | deadline más pequeño | **id de coder más alto** |

## 2. Cómo funciona hoy (el camino de una petición)

Para saber dónde meter mano, conviene recordar por dónde pasa una petición de
dongle:

1. **`src/dongle/build_request.c`**: el coder construye su petición
   (`t_request`). Rellena tres campos: `ticket` (número de llegada),
   `key` (la prioridad) y `coder` (su id).
2. **`src/policy/get_request_key.c`**: calcula la `key`. En `fifo` la key es
   el ticket. En `edf` es el deadline: `last_compile_start + time_to_burnout`.
3. **`src/policy/is_request_before.c`**: es **el árbitro**. Recibe dos
   peticiones y dice si la `a` va antes que la `b`. Primero mira la `key` y,
   si empatan, el `ticket`.
4. **`src/heap/push_request.c` / `pop_request.c`**: el heap de cada dongle usa
   `is_request_before` para ordenarse. La cabeza del heap es quien recibe el
   dongle.

La clave está en el paso 3: **toda la decisión de quién va antes vive en una
sola función**. Si cambio el desempate ahí, el heap entero se reordena solo con
la nueva regla.

## 3. Archivos a tocar

**Solo uno:** `coders/src/policy/is_request_before.c`.

Y por qué no hace falta tocar nada más:

- **`coders/main.h`**: el struct `t_request` ya tiene el campo `int coder`
  con el id del coder. No hay que añadir nada.
- **`build_request.c`**: ya rellena `req.coder = coder->id;`. El dato ya
  llega al árbitro.
- **`get_request_key.c`**: el deadline sigue siendo el primer criterio, así
  que la key no cambia.
- **El heap**: no sabe nada de reglas, solo pregunta a `is_request_before`.

> [!tip] Frase para el evaluador
> "Toda la política de orden está en una sola función comparadora. El heap no
> sabe nada de reglas, solo pregunta quién va antes. Por eso el cambio es de
> una línea."

## 4. Paso a paso

### Paso 1: abrir el archivo

```sh
vim coders/src/policy/is_request_before.c
```

### Paso 2: el código de antes

```c
int	is_request_before(const t_request *a, const t_request *b)
{
	if (a == NULL || b == NULL)
		return (0);
	if (a->key < b->key)
		return (1);
	if (a->key > b->key)
		return (0);
	return (a->ticket < b->ticket);
}
```

### Paso 3: el código de después

Solo cambia la última línea:

```c
int	is_request_before(const t_request *a, const t_request *b)
{
	if (a == NULL || b == NULL)
		return (0);
	if (a->key < b->key)
		return (1);
	if (a->key > b->key)
		return (0);
	return (a->coder > b->coder);
}
```

En formato diff:

```diff
-	return (a->ticket < b->ticket);
+	return (a->coder > b->coder);
```

### Paso 4: el porqué de cada parte

- **Las dos comparaciones de `key` se quedan igual.** Son las que hacen que
  EDF sea EDF: si `a` tiene un deadline más pequeño, va primero, y si lo tiene
  más grande, va después. Solo llegamos a la última línea cuando los deadlines
  son **iguales**, que es justo el caso del empate.
- **`ticket` pasa a ser `coder`.** El ticket era "quién pidió antes". Ahora lo
  que importa es "quién tiene el id más alto", y ese dato está en `coder`.
- **`<` pasa a ser `>`.** La función responde a "¿va `a` antes que `b`?". Con
  `>`, `a` va antes cuando su id es **mayor**. Si dejara `<`, ganaría el id más
  bajo, que es justo lo contrario de lo que me piden. **Este es el error más
  fácil de cometer, así que lo reviso dos veces.**
- **El `NULL` check se queda.** Es la validación defensiva de siempre y no
  tiene nada que ver con el cambio.

### Paso 5: ¿y FIFO? No se rompe

En `fifo` la key **es** el propio ticket, y los tickets nunca se repiten (los
reparte `take_ticket` con un mutex). Así que en FIFO dos peticiones nunca
empatan en `key`, la última línea no se ejecuta nunca y FIFO se comporta
exactamente igual que antes.

### Paso 6: ¿hace falta desempate extra si dos ids son iguales?

No. Los ids son únicos (de `1` a `number_of_coders`), y un coder solo tiene
una petición viva a la vez en cada heap. El empate en id no puede pasar, así
que el orden queda totalmente definido.

### Paso 7: header y Norm

- Actualiza la línea `Updated:` del header 42 (si usas el plugin de vim, se
  actualiza sola al guardar).
- La función sigue con 7 líneas, sin variables nuevas y con tabs. Pasa la Norm
  (lo he comprobado: `is_request_before.c: OK!`).

## 5. Cómo comprobarlo

```sh
make -C coders
norminette coders/src/policy/is_request_before.c
./coders/codexion 3 1000 200 100 100 2 0 edf
```

**Qué tiene que salir.** Con 3 coders, al principio nadie ha compilado, así
que todos tienen el mismo deadline. El coder 1 coge sus dongles en `t=0`. En
`t=200` suelta los dongles y esperan el 2 y el 3, **empatados**:

```text
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
200 3 has taken a dongle      <- gana el 3 (id más alto)
200 3 has taken a dongle
200 3 is compiling
...
400 2 has taken a dongle      <- el 2 va después
```

Con el código de antes, en `t=200` entraría el 2, que había pedido antes que
el 3. Ese es el contraste que se ve en vivo.

> [!note] ¿Por qué el 1 empieza en `t=0` y no el 3?
> En `t=0` no hay cola: el primero que llega encuentra los dongles libres y se
> los queda. El desempate solo actúa cuando hay varios **esperando** en el
> mismo heap. Es justo lo que pasa en `t=200`.

**Tests del repo.** Si corro `make check` con el cambio, fallan
`tests/acceptance/test_4.1.1.c` (AC2 y AC4) y `test_4.3.1.c` (AC4). Es lo
esperado: esos tests exigen la regla antigua (desempate por ticket).
`tests/` es material de desarrollo y no se entrega, así que en la defensa no
cuenta. Si me lo preguntan: "los tests documentan la regla antigua; con el
requisito nuevo habría que actualizarlos para que pidan id más alto".

## 6. Posibles preguntas del evaluador

- **¿Por qué no cambiaste `get_request_key`?** Porque la key sigue siendo el
  deadline. Si metiera el id dentro de la key, mezclaría dos criterios y podría
  hacer que un coder con más prisa perdiera contra uno con id alto. El id solo
  debe decidir **cuando hay empate**.
- **¿Puede provocar inanición (starvation)?** Mucho menos de lo que parece. El
  id solo decide en empates exactos. Cuando el coder que pierde sigue
  esperando, su deadline no se mueve, mientras que el ganador, al compilar,
  empuja el suyo hacia delante. En la siguiente ronda el que perdió tiene el
  deadline más pequeño y gana por EDF puro.
- **¿Es thread-safe?** Sí. `is_request_before` solo lee dos structs y se llama
  con el mutex del dongle cogido (dentro de `push_request`/`pop_request`). No
  he añadido estado compartido.
- **¿Y en FIFO?** No cambia nada: las keys son tickets únicos y nunca empatan
  (paso 5).

## 7. Checklist para la defensa

- [ ] Abrir `coders/src/policy/is_request_before.c`.
- [ ] Dejar intactas las dos comparaciones de `key`.
- [ ] Cambiar la última línea por `return (a->coder > b->coder);`.
- [ ] Comprobar que es `>` (id **más alto**) y no `<`.
- [ ] Guardar y actualizar el header.
- [ ] `make -C coders`: compila sin warnings (`-Wall -Wextra -Werror`).
- [ ] `norminette coders/src/policy/is_request_before.c`: OK.
- [ ] `./coders/codexion 3 1000 200 100 100 2 0 edf`: en `t=200` entra el 3.
- [ ] Explicar por qué FIFO no cambia y por qué no hay inanición.
