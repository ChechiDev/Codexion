---
aliases:
  - Documentación del proyecto
  - Codexion explicado
tags:
  - codexion
  - 42
  - concurrencia
  - defensa
author: sperez-l
---

# Codexion: el proyecto explicado task a task

> [!abstract] Qué es este documento
> Es la guía para entender (y defender) Codexion de principio a fin. Cada
> sección grande es una **task** del plan (`N.M`). Dentro de cada task voy
> subtask por subtask (`N.M.K`): primero cómo se planteó y por qué, y después
> cada función como la explicaría delante de un evaluador de 42.
>
> El código manda: todo lo que se cuenta aquí describe el código tal y como
> está hoy en `coders/`. Cuando una función cambió más tarde por un issue
> (ISS-14, ISS-32, ISS-34, ISS-46), lo verás marcado con una nota.

## Antes de empezar: la foto general

Codexion es una variante del clásico problema de los filósofos. Aquí, en vez
de filósofos y tenedores, hay **coders** sentados en círculo y **dongles**
USB entre ellos. Para compilar, un coder necesita los dos dongles que tiene a
los lados. Después depura, refactoriza y vuelve a empezar. Si pasa
`time_to_burnout` milisegundos sin empezar a compilar, se quema (*burn out*)
y la simulación se para.

Lo que hace interesante el proyecto:

- **Cada coder es un hilo** (`pthread`), y hay un **hilo monitor** aparte que
  vigila los burnouts.
- **Cada dongle tiene su mutex, su variable de condición y una cola de
  prioridad** (un heap) que decide a quién le toca, según el scheduler:
  `fifo` (orden de llegada) o `edf` (el que tiene el plazo más cercano).
- **Hay cooldown**: un dongle recién soltado no se puede volver a coger hasta
  que pasan `dongle_cooldown` ms.
- **Los logs no se pueden mezclar** y, una vez parada la simulación, no puede
  salir ninguna línea más (salvo la propia de `burned out`).

Uso:

```text
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

### Cómo está organizado el código

Todo el entregable vive en `coders/` (lo exige el subject): `Makefile`,
`main.c`, `main.h` y `src/<módulo>/*.c`. Los módulos siguen capas, y las
dependencias siempre apuntan hacia dentro:

| Capa | Módulos | Qué puede usar |
|---|---|---|
| Núcleo (puro) | `utils`, `parsing`, `heap`, `policy` | Solo tipos de `main.h` y `strcmp`/`strlen`/`malloc`/`free`. Nada de hilos, reloj ni salida. |
| Adaptadores | `time`, `log`, `state`, `init`, `cleanup` | Núcleo + pthread, reloj y `write`. |
| Orquestación | `dongle`, `coder`, `monitor`, `lifecycle` | Núcleo + adaptadores. |
| Interfaz | `main.c`, `cli` | Todo lo anterior. |

> [!tip] Por qué capas
> Así el parser y el heap se pueden probar sin hilos ni tiempo, y cuando algo
> falla sabes en qué piso buscar. En la defensa es una buena forma de
> explicar el proyecto de fuera hacia dentro: `main` → `run_simulation` →
> hilos → dongles → heap.

### Las convenciones que verás en todas las funciones

- **Una función, un archivo**, con el mismo nombre.
- **Prefijos con significado**: `get_` lee, `set_` cambia un campo, `is_`
  responde sí/no, `init_`/`free_`/`destroy_` gestionan el ciclo de vida,
  `run_` es la rutina de un hilo.
- **Códigos de retorno**: `0` bien, `-1` error. Las funciones que pueden
  encontrarse la simulación parada devuelven `0` / `1` (parada normal) /
  `-1` (error de verdad). Ese contrato de tres valores lo fueron ganando
  `sleep_ms` (ISS-32), `print_state` (ISS-34) y `set_compile_start` (ISS-46).
- **Sin variables globales**: todo el estado viaja en un `t_sim` que se pasa
  por puntero.

---

# Fase 01: el esqueleto que compila

La idea de esta fase era tener desde el primer día un `coders/` que compila,
pasa la Norm y respeta las reglas del Makefile, aunque todavía no haga nada
útil. Así cada subtask posterior solo añade piezas a algo que ya funciona.

## 1.1 Deliverable build

### 1.1.1 Deliverable Makefile

**Cómo se planteó.** El subject pide un Makefile con `cc`, las flags
`-Wall -Wextra -Werror`, las reglas `$(NAME)`, `all`, `clean`, `fclean` y `re`,
y que **no haga relink**. Le añadimos `-pthread` porque el proyecto usa hilos.

#### `coders/Makefile`

```make
NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I.
OBJ = $(SRC:.c=.o)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c main.h
	$(CC) $(CFLAGS) -c $< -o $@
```

- `SRC` es la lista explícita de todos los `.c`. El subject no prohíbe
  `wildcard`; listarlos a mano es una decisión del proyecto para que el
  Makefile diga exactamente qué se compila.
- **Por qué no hace relink:** `$(NAME)` depende de los `.o`, y cada `.o` de
  su `.c` y de `main.h`. Si nada ha cambiado, `make` ve que el ejecutable es
  más nuevo que todo y no hace nada. Si tocas `main.h`, se recompila todo,
  que es lo correcto porque todos los `.c` lo incluyen.
- `-I.` permite escribir `#include "main.h"` desde cualquier subcarpeta.
- `re` es `fclean` + `all`; `clean` borra los `.o` y `fclean` además el
  binario.

> [!question] Pregunta de defensa: "¿Cómo demuestras que no hace relink?"
> Ejecuto `make` dos veces seguidas. La segunda tiene que decir que no hay
> nada que hacer, sin ninguna línea de `cc`.

## 1.2 Header and entry point

### 1.2.1 Configuration types

**Cómo se planteó.** Antes de parsear nada, necesitábamos un sitio donde
guardar los 8 argumentos. Se creó `main.h` como **único header** del proyecto,
con un guard (`#ifndef MAIN_H`) y la estructura de configuración.

#### `t_config`

```c
typedef struct s_config
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	t_mode	mode;
}	t_config;
```

Es simplemente "los argumentos ya validados", con nombres que copian los del
subject para que nadie tenga que traducir. Todos son `int` porque el parser
garantiza que caben (máximo `INT_MAX`).

### 1.2.2 Rename scheduling policy to mode

**Cómo se planteó.** Al principio el campo del scheduler se llamaba de otra
forma; aquí se renombró a `mode` con el tipo `t_mode`, para que el nombre
fuera corto y consistente en todo el código.

#### `t_mode`

```c
typedef enum e_mode { MODE_FIFO, MODE_EDF }	t_mode;
```

- `MODE_FIFO`: los dongles se dan por orden de llegada.
- `MODE_EDF`: se dan al coder con el plazo de burnout más cercano.

Usar un `enum` en vez de guardar el string `"fifo"`/`"edf"` hace que, después
de parsear, el resto del programa compare enteros y no cadenas.

### 1.2.3 Argument count and usage

**Cómo se planteó.** La primera validación posible: si no hay exactamente 8
argumentos, ni lo intentamos. Se imprime el uso por `stderr` y se sale con
error.

#### `main` (primera versión)

Comprobaba `argc != 9` (el nombre del programa + 8 argumentos), llamaba a
`print_usage` y devolvía `EXIT_USAGE` (1). Ese trozo sigue igual en el `main`
actual (ver [[#9.5.2 Main runs simulation]]).

#### `print_usage`

```c
void	print_usage(void)
```

Escribe por `stderr` la línea de uso y que el scheduler es `fifo` o `edf`.
Al principio estaba dentro de `main.c`; en la 2.2.1 se movió a `src/cli/`, y
en la 10.2.3 pasó de `fprintf` a `write` (te cuento por qué allí).

---

# Fase 02: parsear y validar los argumentos

La regla de oro: **cualquier argumento inválido se rechaza antes de reservar
memoria o crear hilos**. Y los mensajes de error tienen que decir qué
argumento está mal.

Reglas que decidimos (estrictas):

- Se toleran espacios alrededor de los números (los seis espacios de C:
  espacio, `\t`, `\n`, `\v`, `\f`, `\r`), pero no dentro.
- Solo dígitos: nada de signo, decimales, hex ni cadena vacía. Los ceros a la
  izquierda se permiten (`0007` vale 7).
- Máximo `INT_MAX`, con un parser que detecta el desbordamiento **antes** de
  que ocurra.
- `number_of_coders >= 1` y `time_to_burnout >= 1`; el resto `>= 0`.
- El scheduler tiene que ser exactamente `fifo` o `edf`, sensible a
  mayúsculas y sin recortar espacios.

## 2.1 Parsing primitives

### 2.1.1 Trim span

**Cómo se planteó.** Hacía falta "quitar los espacios de los lados" sin copiar
el string ni modificar `argv`. La solución fue devolver una **vista**: un
puntero al primer carácter útil y una longitud.

Esta primera versión se llamaba `trim_span` y usaba un tipo `t_span`. Más
tarde (2.2.4 a 2.2.7) se sustituyó por `trim_slice` / `t_slice`, que es lo
que queda hoy. La idea es exactamente la misma; ver [[#2.2.4 Trim slice]].

### 2.1.2 Parse number

**Cómo se planteó.** Convertir un argumento en `int` con todas las reglas de
arriba, y sin usar `atoi` (que no detecta errores ni desbordamientos).

#### `parse_number`

```c
int	parse_number(const char *str, int *out)
```

1. Recorta los espacios con `trim_slice`. Si queda vacío, error.
2. Recorre la vista carácter a carácter: si alguno no es dígito (`is_digit`),
   error.
3. Antes de hacer `value * 10 + digit`, comprueba
   `value > (INT_MAX - digit) / 10`. Si es así, la operación se pasaría de
   `INT_MAX`, así que devuelve error **sin** llegar a desbordar.
4. Solo si todo es válido escribe en `*out`. Si falla, `*out` no se toca.

Devuelve `0` o `-1`.

> [!question] Pregunta de defensa: "¿Por qué no `atoi`?"
> `atoi("abc")` devuelve 0 y `atoi("99999999999")` tiene comportamiento
> indefinido. No hay forma de distinguir un error de un 0 de verdad. Con mi
> parser, cualquier cosa rara es un error explícito.

## 2.2 Apply project conventions

**Cómo se planteó la task.** Después de las primeras piezas, revisamos que
todo siguiera las convenciones del proyecto (capas, un archivo por función,
header ordenado). Casi todas estas subtasks son refactors sin cambio de
comportamiento: mueven, renombran o extraen código, y los tests anteriores
demuestran que todo sigue funcionando igual.

### 2.2.1 Move print usage

Se movió `print_usage` de `main.c` a `src/cli/print_usage.c`. Motivo: `main`
solo coordina; los textos que ve el usuario son cosa de la capa `cli`.

### 2.2.2 Move trim span

Se movió `trim_span` de `src/parsing/` a `src/utils/`. Motivo: recortar
espacios no sabe nada de argumentos, es una utilidad genérica. El parser usa
utilidades, pero las utilidades no dependen del parser.

### 2.2.3 Header sections

Se ordenó `main.h` en secciones con título (`Macros`, `Types`, `Utils`,
`Parsing`, `Time`, `State`, `Log`, `Policy`, `Heap`, `Init`, `Cleanup`,
`Dongle`, `Coder`, `Monitor`, `Lifecycle`, `CLI`). No cambia la API; es para
que quien lo lea encuentre las cosas siguiendo el orden de las capas.

### 2.2.4 Trim slice

**Cómo se planteó.** Se creó la versión definitiva del recorte con un tipo
propio en `main.h`.

#### `t_slice`

```c
typedef struct s_slice
{
	const char	*start;
	int			len;
}	t_slice;
```

#### `trim_slice`

```c
t_slice	trim_slice(const char *str)
```

- Avanza `str` mientras `is_space` sea cierto (recorta por la izquierda).
- Calcula la longitud con `strlen` y retrocede `end` mientras el último
  carácter sea espacio (recorta por la derecha).
- Devuelve `{start, len}`. Con `NULL` devuelve `{NULL, 0}`.

Ejemplo: `"   0007  "` → vista que empieza en el primer `0` con `len = 4`.
No copia nada ni reserva memoria: el string original sigue intacto. Un
espacio en medio (`"4 2"`) se queda dentro de la vista, y luego
`parse_number` lo rechaza porque no es dígito.

### 2.2.5 Use trim slice

`parse_number` pasó de usar `trim_span` a usar `trim_slice`. Mismo
comportamiento, distinta función.

### 2.2.6 Move trim logic

La lógica de recorte se quedó en un solo sitio, `trim_slice`, para que no
hubiera dos algoritmos iguales.

### 2.2.7 Remove trim span

Se borró `trim_span` y `t_span`, que ya no usaba nadie. Una sola forma de
recortar en todo el proyecto.

### 2.2.8 Is space

#### `is_space`

```c
int	is_space(char c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
}
```

Los caracteres del `\t` (9) al `\r` (13) son justo `\t`, `\n`, `\v`, `\f` y
`\r`. Junto con el espacio, son los seis que `isspace` reconoce en el locale
de C. Como `isspace` no está en la lista de funciones permitidas, la
escribimos nosotros.

### 2.2.9 Use is space

`trim_slice` pasó a usar `is_space` en vez de tener su propia comparación.
Una única definición de "espacio".

### 2.2.10 Is digit

#### `is_digit`

```c
int	is_digit(char c)
{
	return (c >= '0' && c <= '9');
}
```

Lo mismo que con `is_space`: `isdigit` no está permitida.

### 2.2.11 Use is digit

`parse_number` pasó a usar `is_digit`. Una única definición de "dígito".

## 2.3 Config parser

### 2.3.1 Get arg minimum

**Cómo se planteó.** Cada argumento tiene un mínimo distinto. En vez de
repartir esos números por el código, una función los centraliza.

#### `get_arg_minimum`

```c
int	get_arg_minimum(int position)
```

- Posiciones 1 y 2 (`number_of_coders`, `time_to_burnout`): mínimo `1`.
  Sin coders no hay simulación, y un burnout de 0 ms no tiene sentido.
- Posiciones 3 a 7 (tiempos, compilaciones requeridas y cooldown): mínimo `0`.
- Cualquier otra posición: `-1` (no es un argumento numérico).

La usan el parser (para validar) y `print_arg_error` (para decir el rango en
el mensaje), así que los dos nunca pueden contradecirse.

### 2.3.2 Parse mode

#### `parse_mode`

```c
int	parse_mode(const char *str, t_mode *out)
```

Compara con `strcmp` exactamente contra `"fifo"` y `"edf"` y guarda
`MODE_FIFO` o `MODE_EDF`. Cualquier otra cosa (`"FIFO"`, `" edf"`, `""`) es
`-1`. Aquí **no** recortamos espacios a propósito: el subject dice que el
valor tiene que ser exactamente uno de los dos.

### 2.3.3 Parse config

**Cómo se planteó.** Juntar todo: validar los 8 argumentos y, si alguno
falla, saber **cuáles** para poder decirlo.

#### `parse_config`

```c
int	parse_config(char **argv, t_config *config, int *invalid)
```

- Un helper `set_field_addresses` prepara un array de punteros a los campos
  de `config`, indexado por posición (1 a 7). Así se puede recorrer con un
  bucle en vez de escribir siete bloques iguales.
- Para cada posición, `parse_field` llama a `parse_number` y comprueba el
  mínimo con `get_arg_minimum`.
- Guarda en `invalid[position]` un `1` si esa posición falló y un `0` si no.
  La posición 8 es el scheduler, con `parse_mode`.
- Devuelve el **número** de argumentos inválidos (0 si todo está bien).

> [!note] Cambió en ISS-14
> La primera versión devolvía una **máscara de bits** (bit 1 = argumento 1
> inválido, etc.). En ISS-14 se cambió por el array `invalid[9]`, cuyos
> índices coinciden con las posiciones. Es mucho más fácil de leer y de
> defender: `invalid[3]` significa "el tercer argumento está mal", sin hacer
> operaciones de bits.

> [!question] Pregunta de defensa: "¿Por qué validas todos y no paras en el primero?"
> Porque así el usuario ve de una vez todos los argumentos que tiene mal,
> en vez de corregirlos uno a uno.

## 2.4 Entry point validation

### 2.4.1 Print argument error

#### `print_arg_error`

```c
void	print_arg_error(int position)
```

- Posiciones 1 a 7: con `fprintf` a `stderr` escribe
  `Error: <nombre> must be an integer from <mínimo> to <INT_MAX>`. El nombre
  sale de un array estático local y el mínimo de `get_arg_minimum`.
- Posición 8: `Error: scheduler must be fifo or edf`, con `write` (ver la
  10.2.3 para el porqué).

El array de nombres es `static const` **dentro** de la función: no es una
variable global, que el subject prohíbe.

### 2.4.2 Validate arguments in main

**Cómo se planteó.** Conectar el parser con `main`: si `parse_config` dice
que hay errores, imprimir uno por argumento inválido, después el uso, y salir
con código 1. Todo antes de tocar memoria o hilos.

#### `print_arg_errors` (estática en `main.c`)

Recorre `invalid[1..8]` y llama a `print_arg_error` para cada posición
marcada. El `main` completo está en [[#9.5.2 Main runs simulation]].

---

# Fase 03: el tiempo y los logs

Tres piezas básicas que usa todo lo demás: un reloj en milisegundos, un
`sleep` que se entera de cuándo se para la simulación y un logger que nunca
mezcla líneas ni escribe después del final.

## 3.1 Clock

### 3.1.1 Get time ms

#### `get_time_ms`

```c
long	get_time_ms(void)
```

Llama a `gettimeofday` (el único reloj permitido por el subject) y convierte
segundos + microsegundos en milisegundos:
`tv_sec * 1000 + tv_usec / 1000`. Si `gettimeofday` falla, devuelve `-1`.
Es `long` porque los milisegundos desde 1970 no caben en un `int`.

### 3.1.2 Get elapsed ms

#### `get_elapsed_ms`

```c
long	get_elapsed_ms(long start_ms)
```

Devuelve cuántos ms han pasado desde `start_ms`, que es el instante de
arranque de la simulación. Esto es lo que se imprime como timestamp en cada
log. Dos detalles:

- Si el reloj falla, `-1`.
- Si `now < start_ms` (el reloj del sistema saltó hacia atrás), devuelve `0`
  en vez de un número negativo. Un timestamp negativo en el log no tendría
  sentido.

## 3.2 Stop state

**Cómo se planteó la task.** Hace falta una bandera `stopped` compartida por
todos los hilos. Como varios la leen y alguno la escribe, va protegida por
`stop_mutex`. Si no, sería una data race.

### 3.2.1 Is stopped

#### `is_stopped`

```c
int	is_stopped(t_sim *sim)
```

Bloquea `stop_mutex`, copia `stopped` y desbloquea. Devuelve `1` si está
parada.

**Detalle importante:** si falla el lock o el unlock, devuelve `1`. Es la
opción segura: ante la duda, el hilo se para en vez de seguir trabajando con
un estado que no ha podido leer bien.

### 3.2.2 Set stopped

#### `set_stopped`

```c
int	set_stopped(t_sim *sim)
```

Pone `stopped = 1`. Fíjate en que bloquea **primero `log_mutex` y después
`stop_mutex`**. Así, si otro hilo está a mitad de escribir una línea de log
(con `log_mutex` cogido), la parada espera a que termine. Nunca se para la
simulación en medio de una línea, y después de parar ya nadie puede escribir.

> [!tip] Orden de mutex
> En todo el proyecto, cuando hacen falta los dos, se cogen en el orden
> `log_mutex` → `stop_mutex`. Siempre el mismo orden, así que estos dos no
> pueden provocar un deadlock.

## 3.3 Sliced sleep

### 3.3.1 Sleep ms

**Cómo se planteó.** Si un coder hace `usleep(200000)` para compilar y justo
otro se quema, tardaría hasta 200 ms en enterarse de la parada. La solución
es dormir a **trocitos** y mirar la bandera entre trozo y trozo.

#### `sleep_ms`

```c
int	sleep_ms(t_sim *sim, long ms)
```

- Si `ms <= 0`, vuelve enseguida con `0`.
- Guarda el instante de inicio y entra en un bucle:
  1. Si `is_stopped`, devuelve `1`.
  2. Si ya han pasado `ms`, devuelve `0`.
  3. Si no, duerme un trozo de como mucho `SLEEP_SLICE_US` (500 µs) con
     `usleep` (`get_slice_us` calcula el trozo).
- Si el reloj falla, `-1`.

Mide el tiempo con el reloj en cada vuelta, no sumando trozos: `usleep`
puede dormir más de lo pedido, así que contar trozos acumularía retraso.

> [!note] Cambió en ISS-32
> Antes, una parada devolvía `-1`, igual que un error. En ISS-32 se separaron:
> `0` terminó de dormir, `1` se paró la simulación (algo normal) y `-1` error
> de verdad. Gracias a eso, el coder puede distinguir "se acabó" de "algo se
> ha roto".

## 3.4 Serialized logger

**Cómo se planteó la task.** El subject exige que las líneas no se mezclen y
que la de `burned out` salga en 10 ms como mucho. Se decidió:

- Construir cada línea entera en un buffer local y mandarla con **un solo
  `write`** a la salida estándar, con `log_mutex` cogido.
- **No usar `printf`** para los logs. `printf` usa un buffer interno, y si la
  salida va a un pipe, las líneas pueden tardar en salir (y `fflush` no está
  permitido). Con `write` no hay buffer nuestro de por medio: cada línea
  se entrega al sistema en cuanto se escribe.
- Como no se puede usar `snprintf`, los números se pasan a texto con una
  función propia.

### 3.4.1 Build number text

#### `build_number_text`

```c
int	build_number_text(long n, char *buf)
```

Escribe los dígitos de `n` (>= 0) al principio de `buf` y devuelve cuántos
ha escrito. Un helper `get_digit_count` calcula primero el número de dígitos,
y luego los rellena de derecha a izquierda con `n % 10`. No pone `'\0'` ni
signo. Con `n < 0` o `buf == NULL` devuelve `-1`.

### 3.4.2 Build log line

#### `build_log_line`

```c
int	build_log_line(char *line, long ts, int coder, const char *text)
```

Monta `"<ts> <coder> <texto>\n"` en `line` y devuelve la longitud. Es una
función **pura** (no toca mutex, ni reloj, ni salida), así que se puede
probar sola. Antes de escribir nada valida la entrada con
`is_valid_log_input`:

- `ts >= 0`, `coder >= 1` y el texto no vacío y de `LOG_TEXT_MAX` (32)
  caracteres como mucho;
- el texto sin saltos de línea (`has_line_break`), para que nadie pueda
  "inyectar" una línea falsa en el log.

El buffer tiene `LOG_LINE_MAX` (64) bytes, suficiente para el peor caso.

### 3.4.3 Print state

#### `print_state`

```c
int	print_state(t_sim *sim, int coder, t_state state)
```

Es la función que escribe los cuatro estados normales:

| `t_state` | Texto |
|---|---|
| `STATE_TAKEN_DONGLE` | `has taken a dongle` |
| `STATE_COMPILING` | `is compiling` |
| `STATE_DEBUGGING` | `is debugging` |
| `STATE_REFACTORING` | `is refactoring` |

1. Traduce el estado a texto (`get_state_text`). Si el estado o el coder no
   son válidos, `-1` **antes** de bloquear nada.
2. Bloquea `log_mutex`.
3. Lee `stopped` con `stop_mutex` (`get_stopped`). **Si está parada, no
   escribe nada.** Esta es la clave de que no salga ninguna línea después del
   final: la comprobación y la escritura ocurren con el mismo `log_mutex`
   cogido, así que nadie puede parar la simulación entre medias.
4. Si no está parada, escribe con `print_log_line`.
5. Desbloquea `log_mutex`.

Devuelve `0` (escrita), `1` (simulación parada, nada escrito) o `-1` (error).

> [!note] Cambió en ISS-34
> Igual que `sleep_ms` en ISS-32: antes una parada devolvía `-1`. Ahora
> devuelve `1`, así que los que llaman a `print_state` saben si es un final
> normal o un fallo de escritura.

### 3.4.4 Print burnout

#### `print_burnout`

```c
int	print_burnout(t_sim *sim, int coder)
```

La línea de `burned out` es especial: es la única que puede salir "en el
momento de parar". Para que sea la última línea del log:

1. Bloquea `log_mutex`.
2. `set_stopped_once`: con `stop_mutex`, pone `stopped = 1` solo si todavía
   era 0. Si ya estaba parada, devuelve `-1` (alguien ha parado antes).
3. Solo si esta llamada ha hecho la parada, escribe `burned out`.
4. Desbloquea `log_mutex`.

Parar y escribir ocurren en la misma sección crítica de `log_mutex`. Ningún
otro hilo puede colar un `is compiling` detrás de la línea de burnout, porque
cuando consiga `log_mutex` ya verá `stopped = 1`.

### 3.4.5 Declare print log line

**Cómo se planteó.** `print_state` y `print_burnout` hacían lo mismo al final
(leer la hora, montar la línea y escribirla). Primero se declaró en `main.h`
una función común, `print_log_line`, con su contrato documentado.

### 3.4.6 Extract print log line

Después se movió ese código compartido a su propia función, y las dos la
usan.

#### `print_log_line`

```c
int	print_log_line(t_sim *sim, int coder, const char *text)
```

- **Precondición:** quien la llama ya tiene `log_mutex`. Ella no bloquea
  nada ni mira `stopped`; eso es trabajo de quien llama.
- Lee el timestamp con `get_elapsed_ms`, monta la línea con `build_log_line`
  y la manda con **un** `write(1, ...)`.
- Devuelve `0` solo si `write` escribió la línea entera; si no, `-1` (no
  reintenta).

Separar "¿puedo escribir?" (`print_state`/`print_burnout`) de "escribe"
(`print_log_line`) hace que cada función tenga un solo trabajo.

---

# Fase 04: la cola de prioridad (heap)

**Cómo se planteó.** El subject pide que la cola se gestione con una cola de
prioridad para decidir a quién se da cada dongle. Hicimos un **min-heap
binario** a mano, guardado en un array. Cada dongle tendrá el suyo (fase 05).

#### `t_request` y `t_heap`

```c
typedef struct s_request { long key; long ticket; int coder; }	t_request;
typedef struct s_heap { t_request *items; int size; int capacity; }	t_heap;
```

- `key`: la prioridad (menor = antes).
- `ticket`: número de llegada, para desempatar.
- `coder`: quién pide.

En el array, los hijos del nodo `i` están en `2i+1` y `2i+2`, y su padre en
`(i-1)/2`. La raíz `items[0]` es siempre la petición más prioritaria.

## 4.1 Request order

### 4.1.1 Is request before

#### `is_request_before`

```c
int	is_request_before(const t_request *a, const t_request *b)
```

Devuelve `1` si `a` va antes que `b`: menor `key` primero y, a igual `key`,
menor `ticket`. Como los tickets son únicos, esto es un **orden total
estricto**: nunca hay dos peticiones "empatadas". Esto es importante para la
demostración de que no hay deadlock ni inanición (fase 06).

### 4.1.2 Get request key

#### `get_request_key`

```c
long	get_request_key(t_mode mode, long ticket, long last_compile_start,
			int time_to_burnout)
```

Calcula la prioridad según el scheduler:

- **fifo**: `key = ticket`. El que llegó antes, primero.
- **edf**: `key = last_compile_start + time_to_burnout`, que es justo el
  instante en que ese coder se quemaría. El plazo más cercano, primero.

Valida la entrada (ticket negativo, `time_to_burnout < 1`, etc.) y comprueba
que la suma no desborde `LONG_MAX`. En cualquier error, `-1`.

## 4.2 Heap storage

### 4.2.1 Init heap

#### `init_heap`

```c
int	init_heap(t_heap *heap, int capacity)
```

Reserva con `malloc` un array de `capacity` peticiones y deja `size = 0`.
Antes de multiplicar comprueba que `capacity * sizeof(t_request)` no desborde
`SIZE_MAX`. Con capacidad < 1 o sin memoria, `-1`.

### 4.2.2 Free heap

#### `free_heap`

```c
void	free_heap(t_heap *heap)
```

Libera el array y deja el heap a cero (`items = NULL`, `size` y `capacity` a
0). Así, si alguien lo llama dos veces, el segundo `free(NULL)` es inofensivo.

## 4.3 Heap operations

### 4.3.1 Push request

#### `push_request`

```c
int	push_request(t_heap *heap, const t_request *req)
```

Mete la petición al final del array y la hace **subir** (`sift_up`): mientras
vaya antes que su padre, se intercambian. Es O(log n). Si el heap está lleno,
`-1`: no crece, porque sabemos de antemano cuántas peticiones puede haber
(ver `init_dongle`).

### 4.3.2 Get head request

#### `get_head_request`

```c
int	get_head_request(const t_heap *heap, t_request *out)
```

Copia la raíz (la petición más prioritaria) sin sacarla. Si está vacío, `-1`.
Es lo que usa `is_dongle_ready` para preguntar "¿me toca a mí?".

### 4.3.3 Pop request

#### `pop_request`

```c
int	pop_request(t_heap *heap, t_request *out)
```

Saca la raíz, pone el último elemento en su sitio y lo hace **bajar**
(`sift_down`): se intercambia con el hijo más prioritario mientras este vaya
antes. También O(log n).

---

# Fase 05: montar y desmontar la simulación

**Cómo se planteó.** Antes de lanzar hilos hay que crear todo el estado:
dongles (mutex, cond, heap), coders (mutex propio) y los mutex globales.
Y hay que poder **deshacerlo todo**, también si falla a medias (un `malloc`
o un `pthread_mutex_init` que falla en el tercer dongle), sin fugas.

#### `t_dongle`, `t_coder` y `t_sim`

```c
typedef struct s_dongle
{
	pthread_mutex_t	mutex;        // protege todo el dongle
	pthread_cond_t	cond;         // para esperar sin busy-wait
	t_heap			heap;         // cola de peticiones de este dongle
	int				available;    // 1 libre, 0 cogido
	long			released_at;  // cuándo se soltó (-1 = nunca)
}	t_dongle;

typedef struct s_coder
{
	int				id;                  // 1..N, el que sale en el log
	int				first_dongle;        // índice menor
	int				second_dongle;       // índice mayor
	pthread_mutex_t	mutex;               // protege los dos campos de abajo
	long			last_compile_start;  // ms desde el inicio
	int				compile_count;
	pthread_t		thread;
	struct s_sim	*sim;                // vuelta a la simulación
}	t_coder;
```

`t_sim` lo agrupa todo: la configuración, `start_ms`, las banderas `stopped`
y `failed`, el hilo monitor, el contador de tickets, los arrays de dongles y
coders y los tres mutex globales (`stop_mutex`, `log_mutex`, `ticket_mutex`).

## 5.1 Seat mapping

### 5.1.1 Get dongle pair

#### `get_dongle_pair`

```c
int	get_dongle_pair(int seat, int n, int *first, int *second)
```

Decide qué dos dongles usa el coder del asiento `seat` (0 a n-1). Tiene el de
su asiento y el de la derecha; el último asiento usa el `0` como derecha,
porque están en círculo. Lo importante es que siempre devuelve **el menor en
`first` y el mayor en `second`**. Más adelante los mutex de los dongles se
cogen siempre en ese orden, y eso evita deadlocks.

Caso especial: con `n = 1`, la derecha del asiento 0 es el 0, así que
`first == second == 0`. Un solo coder tiene un solo dongle, como dice el
subject.

## 5.2 Dongles

### 5.2.1 Init dongle

#### `init_dongle`

```c
int	init_dongle(t_dongle *dongle)
```

Inicializa el mutex, la variable de condición y un heap de **capacidad 2**.
Dos es suficiente porque un dongle solo lo comparten dos coders y cada uno
tiene como mucho una petición pendiente. Deja `available = 1` y
`released_at = -1`, que significa "nunca se ha soltado": al principio no hay
cooldown que esperar.

Si falla a medias, deshace lo que ya había creado (por ejemplo, si falla el
heap, destruye la cond y el mutex) antes de devolver `-1`.

### 5.2.2 Destroy dongle

#### `destroy_dongle`

Libera el heap y destruye la cond y el mutex. Aunque falle uno, sigue con
los demás y al final devuelve `-1` si alguno falló. Así no se queda nada
colgando por un error intermedio.

### 5.2.3 Free dongles

#### `free_dongles`

```c
int	free_dongles(t_dongle *dongles, int count)
```

Destruye los primeros `count` dongles (en orden inverso) y libera el array.
El `count` es la clave de la limpieza parcial: si falló el dongle 3, se llama
con `count = 3` y solo se destruyen los que sí se habían inicializado.

### 5.2.4 Init dongles

#### `init_dongles`

Reserva el array de `n` dongles (comprobando el desbordamiento de
`n * sizeof`) e inicializa cada uno. Si el `i`-ésimo falla, llama a
`free_dongles(array, i)` y devuelve `-1`.

## 5.3 Coders

### 5.3.1 Init coder

#### `init_coder`

```c
int	init_coder(t_coder *coder, t_sim *sim, int seat, int n)
```

Calcula sus dongles con `get_dongle_pair`, inicializa su mutex y rellena:
`id = seat + 1` (en el log los coders empiezan en 1),
`last_compile_start = 0` (el plazo cuenta desde el inicio de la simulación,
como dice el subject), `compile_count = 0` y el puntero a `sim`.

### 5.3.2 Destroy coder

#### `destroy_coder`

Destruye el mutex del coder. Es lo único que reserva un coder.

### 5.3.3 Free coders

#### `free_coders`

Mismo patrón que `free_dongles`: destruye los `count` primeros en orden
inverso y libera el array.

### 5.3.4 Init coders

#### `init_coders`

Mismo patrón que `init_dongles`, llamando a `init_coder` para cada asiento y
limpiando con `free_coders(array, i)` si alguno falla.

## 5.4 Simulation

### 5.4.1 Free sim

#### `free_sim`

```c
int	free_sim(t_sim *sim)
```

Libera los coders, después los dongles (poniendo los punteros a `NULL`) y
destruye los tres mutex globales. Igual que las demás, sigue aunque falle
algo y devuelve `-1` al final si hubo algún error.

### 5.4.2 Init simulation

#### `init_simulation`

```c
int	init_simulation(t_sim *sim, const t_config *config)
```

1. Copia la configuración y pone a cero `start_ms`, `stopped`, `failed` y
   `next_ticket`, y los punteros a `NULL`.
2. Inicializa los tres mutex globales (`init_mutexes` deshace los anteriores
   si uno falla).
3. Crea los dongles y los coders. Si algo falla, llama a `free_sim`, que
   puede limpiar un estado a medias porque los punteros sin crear siguen
   siendo `NULL`.

---

# Fase 06: el arbitraje de dongles

Es el corazón del proyecto. Tres ideas lo resumen:

1. **Coger los dos dongles a la vez o ninguno.** Un coder nunca se queda con
   uno mientras espera el otro. Eso rompe la condición de Coffman de
   *hold and wait*, y sin ella no puede haber deadlock.
2. **Una sola petición, en los dos heaps.** El coder mete la misma petición
   (misma `key` y mismo `ticket`) en el heap de cada uno de sus dos dongles.
   Solo los coge cuando es **la cabeza de los dos heaps** y los dos están
   libres y sin cooldown.
3. **Esperar sin busy-wait.** Si no le toca, espera con
   `pthread_cond_timedwait`. Cuando alguien suelta un dongle, hace
   `broadcast` y los que esperan se despiertan a comprobar.

> [!question] Pregunta de defensa: "¿Por qué no hay deadlock ni inanición?"
> Son tres preguntas distintas y conviene separarlas:
>
> - **Deadlock:** nadie retiene un dongle mientras espera otro, y los mutex se
>   cogen siempre en el mismo orden (índice menor primero). No se puede
>   formar un ciclo de esperas.
> - **Inanición:** como el orden `(key, ticket)` es total, el coder que espera
>   con la petición más prioritaria de todas es la cabeza de **sus dos**
>   heaps, y nadie puede quitarle sus dongles. Un vecino sí puede adelantar a
>   un coder que espera: por ejemplo, si cogió su ticket antes pero lo metió
>   en el heap después (el ticket se pide antes de bloquear los dongles), o
>   en edf si su plazo era anterior. Pero los adelantamientos son **finitos**:
>   en cuanto a ese vecino le sirven, su siguiente petición lleva un ticket
>   nuevo (mayor) o un plazo calculado desde una compilación que acaba de
>   empezar (posterior), así que ya va detrás. Cada vecino adelanta como
>   mucho una vez, y la petición que espera acaba siendo la más prioritaria y
>   se sirve. Esto supone que el sistema operativo acaba ejecutando todos los
>   hilos.
> - **Plazos (no quemarse):** eso ya depende de los tiempos; ver la cota de
>   viabilidad en [[#10.2.1 Viability boundary]].

## 6.1 Requests

### 6.1.1 Take ticket

#### `take_ticket`

```c
int	take_ticket(t_sim *sim, long *out)
```

Da números de llegada únicos y crecientes. Con `ticket_mutex` cogido, lee
`next_ticket` y lo incrementa. El mutex es necesario porque varios coders
pueden pedir ticket a la vez, y sin él dos podrían recibir el mismo número.
Comprueba que no llegue a `LONG_MAX`.

### 6.1.2 Build request

#### `build_request`

```c
int	build_request(t_coder *coder, t_request *out)
```

Monta la petición de un coder:

1. Lee su `last_compile_start` con su mutex (`get_last_start`).
2. Pide un ticket.
3. Calcula la `key` con `get_request_key` según el scheduler.
4. Pone `coder = id`.

## 6.2 Readiness

### 6.2.1 Is dongle ready

#### `is_dongle_ready`

```c
int	is_dongle_ready(const t_dongle *dongle, int coder, long now, int cooldown)
```

Responde a "¿puedo coger este dongle ahora?". Tienen que cumplirse las tres
cosas:

1. Mi petición es la cabeza de su heap (`get_head_request`).
2. Está libre (`available == 1`).
3. El cooldown ha pasado: o nunca se ha soltado (`released_at == -1`), o
   `now - released_at >= cooldown`.

Es pura: no bloquea nada. **Quien la llama ya tiene el mutex del dongle.**

### 6.2.2 Get deadline

#### `get_deadline`

```c
int	get_deadline(long ms, struct timespec *out)
```

`pthread_cond_timedwait` no recibe "espera 1 ms", sino "espera hasta tal
hora" (un `timespec` absoluto). Esta función calcula "ahora + `ms`" con
`gettimeofday`, llevando bien los nanosegundos que pasan de un segundo y
comprobando que no desborde.

## 6.3 Acquisition and release

### 6.3.1 Wake dongles

#### `wake_dongles`

```c
int	wake_dongles(t_sim *sim)
```

Hace `pthread_cond_broadcast` en **todos** los dongles (cogiendo el mutex de
cada uno). Se usa al parar la simulación, para que ningún coder se quede
dormido esperando un dongle que ya no va a llegar.

### 6.3.2 Wait for dongles

#### `wait_for_dongles`

```c
int	wait_for_dongles(t_coder *coder)
```

Se llama con los mutex de los **dos** dongles cogidos:

1. Calcula un plazo de 1 ms (`get_deadline`).
2. Suelta el mutex del segundo dongle.
3. Espera con `pthread_cond_timedwait` en la cond del primero (eso suelta
   atómicamente el mutex del primero mientras duerme).
4. Al despertar (por `broadcast` o porque pasó 1 ms), vuelve a coger el
   segundo.

Si devuelve `0`, tiene otra vez los dos cogidos, en el orden correcto
(primero ya lo tiene, luego el segundo). Si algo falla (el reloj, un unlock,
la espera o el relock), intenta soltar lo que tenga y devuelve `-1`. Por eso
`take_dongles` distingue un error "fatal con la liberación ya intentada"
(`-2` en su código interno): en ese caso no vuelve a intentar soltarlos. Los
unlock de esa limpieza pueden fallar y se ignoran, porque el hilo ya va a
parar la simulación de todas formas.

> [!question] Pregunta de defensa: "¿Por qué un timeout de 1 ms si ya hay broadcast?"
> Porque **el fin del cooldown no avisa a nadie**: no hay ningún evento
> cuando pasan los ms del cooldown. Con el timeout, el coder vuelve a
> comprobar cada milisegundo como mucho. No es busy-wait: el hilo está
> dormido en la cond, no dando vueltas en un bucle.

### 6.3.3 Release dongles

#### `release_dongles`

```c
int	release_dongles(t_coder *coder)
```

Suelta los dongles de un coder:

1. `pick_dongles` los ordena (menor y mayor).
2. Coge sus mutex en ese orden.
3. `mark_released`: `available = 1` y `released_at = ahora` (aquí empieza el
   cooldown).
4. `broadcast_pair`: despierta a quien espere en cualquiera de los dos.
5. Desbloquea.

Funciona igual con un solo dongle (`low == high`), sin bloquear el mismo
mutex dos veces.

### 6.3.4 Take dongles

#### `take_dongles`

```c
int	take_dongles(t_coder *coder, int *taken)
```

La función que coge los dos dongles:

1. `push_requests`: monta la petición, coge los dos mutex (primero el menor)
   y mete **la misma** petición en los dos heaps. Se queda con los dos mutex
   cogidos.
2. Bucle de `take_pair`:
   - Si la simulación está parada, sale sin coger nada (`*taken = 0`).
   - Si **los dos** dongles están listos para mí (`is_dongle_ready` en
     ambos), saca mi petición de los dos heaps, marca los dos
     `available = 0` y sale con `*taken = 1`. Los dos de golpe.
   - Si no, `wait_for_dongles` y vuelve a comprobar.
3. Suelta los dos mutex. Los dongles quedan "cogidos" por la bandera
   `available = 0`, no por el mutex: el mutex solo protege la comprobación,
   que dura muy poco.

Si hay un error grave (no puede soltar un mutex, falla el reloj...), para la
simulación y despierta a todos antes de devolver `-1`.

> [!note] Si se para con la petición en cola
> Si la simulación se para mientras espera, la petición se queda en los
> heaps. No pasa nada: la ejecución ha terminado, todos los hilos se unen con
> `join` antes de liberar nada y `free_sim` libera los heaps.

### 6.3.5 Take single dongle

#### `take_single_dongle`

```c
int	take_single_dongle(t_coder *coder, int *taken)
```

La misma lógica para el caso de **un solo coder**, que solo tiene un dongle.
Mete su petición, espera con `pthread_cond_timedwait` de 1 ms hasta que esté
listo, lo saca del heap y lo marca cogido. Tener una versión aparte evita
llenar `take_dongles` de casos especiales.

---

# Fase 07: el hilo de cada coder

## 7.1 Coder state

**Cómo se planteó la task.** El monitor necesita leer `last_compile_start` y
`compile_count` de cada coder mientras el coder los cambia. Para que no haya
data races, cada coder tiene su propio mutex, y solo se escriben a través de
estas dos funciones.

### 7.1.1 Set compile start

#### `set_compile_start`

```c
int	set_compile_start(t_coder *coder)
```

Apunta "empiezo a compilar ahora" en `last_compile_start`, con el mutex del
coder cogido. Lee el reloj **dentro** del mutex: si tuviera que esperar al
mutex, no guardaría una hora vieja (que acortaría su plazo).

- Si el plazo ya se ha alcanzado
  (`last_compile_start <= now - time_to_burnout`), **no** sobrescribe el
  valor y devuelve `1`: el coder ya se ha quemado y no debe compilar.
- Si no, guarda `now` y devuelve `0`.
- Error: `-1`.

> [!warning] Cambió en ISS-46 (bug real que encontró una auditoría)
> Antes guardaba la hora **siempre**. El monitor mira cada ~1 ms. Si el plazo
> de un coder vencía justo después de una pasada del monitor, y ese coder
> conseguía los dongles antes de la siguiente, la hora nueva borraba el plazo
> vencido. El burnout no se notificaba nunca y la simulación seguía, cosa que
> el subject no permite. Ahora la comprobación y la escritura van en la misma
> sección crítica y usan la misma regla que el monitor (`<=`, inclusiva). Un
> plazo vencido ya no se puede borrar.

### 7.1.2 Set compile count

#### `set_compile_count`

```c
int	set_compile_count(t_coder *coder)
```

Suma 1 a `compile_count` con el mutex del coder. Comprueba que no llegue a
`INT_MAX`.

## 7.2 Coder acquisition

### 7.2.1 Take coder dongles

#### `take_coder_dongles`

```c
int	take_coder_dongles(t_coder *coder, int *taken)
```

Junta "coger" y "anunciarlo":

1. `take_pair`: si el coder tiene un solo dongle, llama a
   `take_single_dongle`; si no, a `take_dongles`.
2. Si no ha cogido nada (simulación parada), `*taken = 0`.
3. Si los ha cogido, `print_taken` escribe `has taken a dongle` **una vez
   por dongle** (dos veces normalmente, una con un solo coder), como pide el
   subject.
4. Si no puede imprimir, devuelve los dongles (`release_taken`) para no
   quedárselos.

> [!note] Ajustado en la 9.2.1
> Ahora guarda **por qué** no pudo imprimir. Si `print_state` dio `1`
> (parada), suelta los dongles y devuelve `0` con `*taken = 0`: es un final
> normal. Si dio `-1`, devuelve `-1`: es un error de verdad.

## 7.3 Coder routine

### 7.3.1 Run coder

#### `run_coder`

```c
void	*run_coder(void *arg)
```

Es la función que ejecuta cada hilo coder (la que se pasa a
`pthread_create`). Su ciclo:

```text
sleep_start_turn  (solo una vez al principio, ver 10.1)
loop:
  take_coder_dongles  → "has taken a dongle" x2
  sleep_compile:
      set_compile_start
      "is compiling"
      sleep_ms(time_to_compile)
      set_compile_count
      release_dongles   (suelta los dongles antes de depurar)
  sleep_rest:
      "is debugging"    + sleep_ms(time_to_debug)
      "is refactoring"  + sleep_ms(time_to_refactor)
```

Las funciones de apoyo:

- **Cuota individual:** el bucle no consulta `compile_count`. Un coder que
  ya ha hecho sus N compilaciones sigue trabajando hasta la parada global,
  que decide el monitor cuando **todos** han llegado.
- **`sleep_compile`**: el tramo de compilar. En cuanto un paso devuelve
  algo distinto de `0` (parada `1` o error `-1`), corta y lo devuelve.
- **`sleep_rest`**: depurar y refactorizar; al final mira `is_stopped`.
- **`wait_single`**: con un solo coder, después de coger su único dongle no
  puede compilar nunca. Duerme a trozos hasta que el monitor detecta el
  burnout y para la simulación.
- **`release_run`**: la salida de la rutina. Si el coder tenía dongles
  (`held`), los suelta. Si la salida fue por **error** (`-1`), además marca
  la simulación como fallida (`set_failed`), la para y despierta a todos. Si
  fue una parada normal (`1`), sale sin más.

> [!tip] "Solo el monitor para por un motivo normal"
> Un coder nunca para la simulación porque haya terminado o se haya quemado.
> Eso lo decide el monitor. Un coder solo la para si algo se ha roto (y
> entonces la marca como fallida). Esta regla (fase 09) hace que el código de
> salida del programa sea fiable.

---

# Fase 08: el hilo monitor

## 8.1 Monitor checks

### 8.1.1 Get burned out

#### `get_burned_out`

```c
int	get_burned_out(t_coder *coder, int *burned)
```

Con el mutex del coder, lee `last_compile_start` y la hora actual y calcula
`burned = last <= now - time_to_burnout`. Es decir, el coder se ha quemado
**en cuanto se alcanza** el plazo (regla inclusiva). Es la misma regla que
usa `set_compile_start` desde ISS-46.

### 8.1.2 Get target met

#### `get_target_met`

```c
int	get_target_met(t_sim *sim, int *met)
```

Recorre los coders y comprueba, con el mutex de cada uno, si
`compile_count >= number_of_compiles_required`. Se para en el primero que no
llega. `met = 1` solo si **todos** han llegado. Si se requieren 0
compilaciones, `met = 1` directamente.

## 8.2 Monitor routine

### 8.2.1 Run monitor

#### `run_monitor`

```c
void	*run_monitor(void *arg)
```

La rutina del hilo monitor: un bucle que repite `scan_step` hasta que
devuelve algo distinto de "sigue mirando", y luego `set_monitor_end` cierra
según el resultado.

Cada pasada de **`scan_step`**, en este orden:

1. **¿Ya está parada?** (un coder falló) → sale sin escribir nada.
2. **¿Alguien se ha quemado?** `handle_burnout` → `get_burned_index` busca el
   primer coder quemado (en caso de empate, el de índice más bajo; esa
   decisión está en un solo sitio a propósito). Si lo hay, `print_burnout`
   para la simulación y escribe la línea a la vez.
3. **¿Se ha cumplido el objetivo?** `get_target_met` → si sí, para la
   simulación sin escribir nada.
4. Si no, `usleep(1000)` y otra vuelta.

Mira cada ~1 ms y la línea de burnout sale con un `write` directo, sin
buffer. Eso deja mucho margen frente a los 10 ms del subject, pero no es una
garantía absoluta: cada pasada coge mutexes, el `write` puede tardar y el
sistema operativo puede retrasar el hilo. Lo que sí está probado son los
escenarios de los tests, que lo cumplen.

**`set_monitor_end`**: después de un burnout o del objetivo cumplido,
despierta a todos los coders (`wake_dongles`) para que vean la parada y
terminen. Si salió por error, marca `failed`, para y despierta.

> [!warning] Cambió en ISS-46: primero el burnout, luego el objetivo
> Antes se comprobaba primero el objetivo. Caso problemático: un coder ya ha
> hecho sus N compilaciones. Ojo, eso **no** lo retira: sigue con su ciclo
> (su bucle no mira su propia cuota) y sigue pudiendo quemarse. Mientras
> espera dongles, su plazo vence, y en ese mismo milisegundo otro coder
> termina su última compilación. El monitor
> veía las dos cosas en la misma pasada, salía "limpio" y se tragaba un
> burnout que ya había ocurrido. No se puede saber cuál de los dos pasó
> antes, así que la política segura es **notificar el burnout**: así nunca se
> esconde un plazo vencido.

---

# Fase 09: el ciclo de vida completo

**Cómo se planteó.** Juntarlo todo en `main`: parsear, inicializar, arrancar
hilos, esperarlos, liberar y devolver un código de salida que diga la verdad.
Hubo que decidir qué pasa si algo falla en tiempo de ejecución (un mutex, el
reloj, un `write`).

Decisiones:

- Una bandera `failed`, aparte de `stopped`, para distinguir "terminó bien"
  (burnout u objetivo) de "algo se rompió".
- Si un fallo se propaga hasta `failed`, código de salida `2`
  (`EXIT_RUNTIME`) y el mensaje `Error: simulation failed` por `stderr`.
  No todos los fallos llegan ahí: `is_stopped` trata un error de mutex como
  "parada" sin marcar `failed` (lo prudente es parar), y `sleep_ms` no mira
  el resultado de `usleep` (solo usa el reloj para saber cuánto ha dormido).
- Con 0 compilaciones requeridas, `run_simulation` devuelve 0 sin crear
  hilos ni escribir nada.

## 9.1 Failure state

### 9.1.1 Failure fields

Se añadió el campo `failed` a `t_sim` (protegido por `stop_mutex`) y
`init_simulation` lo pone a `0`.

### 9.1.2 Set failed

#### `set_failed`

```c
int	set_failed(t_sim *sim)
```

Pone `failed = 1` con `stop_mutex` cogido. Solo marca el fallo; parar es
trabajo de `set_stopped`.

## 9.2 Report runtime failures

### 9.2.1 Take keeps print result

`take_coder_dongles` pasó a respetar el resultado de `print_state` (`1`
parada frente a `-1` error). Explicado en [[#7.2.1 Take coder dongles]].

### 9.2.2 Coder reports failure

`run_coder` pasó a llamar a `set_failed` cuando sale por un `-1`, dentro de
`release_run`. Así un fallo dentro de un hilo coder llega hasta el código de
salida del programa.

### 9.2.3 Monitor reports failure

`run_monitor` hace lo mismo: si una de sus comprobaciones falla, marca
`failed`, para y despierta (`set_monitor_end`).

## 9.3 Threads

### 9.3.1 Join threads

#### `join_threads`

```c
int	join_threads(t_sim *sim, int coders)
```

Hace `pthread_join` de los primeros `coders` hilos coder y después del
monitor. Recibe cuántos coders unir porque, si la creación falló a medias,
solo hay que esperar a los que sí se crearon.

### 9.3.2 Start threads

#### `start_threads`

```c
int	start_threads(t_sim *sim)
```

1. Fija `start_ms = ahora`: el "tiempo 0" de todos los logs, justo antes de
   crear hilos.
2. Crea **primero el monitor**, así está vigilando desde el principio.
3. Crea un hilo por coder con `run_coder`.
4. Si falla la creación de un coder (`recover_start`): marca fallo, para,
   despierta y une los hilos ya creados.

Devuelve `0` (todo arrancado), `-1` (falló, pero los hilos están unidos y se
puede liberar) o `-2` (no se pudieron unir los hilos: **no** se debe liberar
la memoria, porque algún hilo podría seguir usándola).

## 9.4 Run simulation

### 9.4.1 Run simulation

#### `run_simulation`

```c
int	run_simulation(t_sim *sim, const t_config *config)
```

El ciclo completo de una ejecución:

1. Si `number_of_compiles_required == 0`, devuelve `0` sin hacer nada.
2. `init_simulation`.
3. `start_threads`. Si da `-1`, libera y devuelve `-1`. Si da `-2`, devuelve
   `-1` **sin** liberar (por lo explicado arriba).
4. `join_simulation`: une todos los hilos, lee `failed`, llama a `free_sim`
   y devuelve `-1` si algo falló. **Excepción:** si falla algún `join`,
   devuelve `-1` **sin** liberar (`join_threads` intenta unir todos los
   hilos igualmente y después informa del error), por la misma razón que el `-2`
   de `start_threads`: algún hilo podría seguir usando esa memoria.

> [!tip] Por qué `t_sim` vive en la pila de `main`
> El `t_sim` se declara en `main` y se pasa por puntero. Si un `join`
> fallara, la memoria del `t_sim` tiene que seguir existiendo mientras algún
> hilo pueda tocarla, y la pila de `main` dura todo el programa.

## 9.5 Entry point

### 9.5.1 Print run error

#### `print_run_error`

Escribe `Error: simulation failed` por `stderr` con `write`.

### 9.5.2 Main runs simulation

#### `main`

```c
int	main(int argc, char **argv)
{
	if (argc != 9)                                → uso, return 1
	if (parse_config(argv, &config, invalid))     → errores + uso, return 1
	if (run_simulation(&sim, &config) == -1)      → "simulation failed", return 2
	return (0);
}
```

Tres posibles salidas: `1` si la entrada es inválida (nunca se crea nada),
`2` si la simulación falló en tiempo de ejecución y ese fallo se registró en
`failed` (ver los matices en la introducción de la fase 09) y `0` si terminó bien
(burnout u objetivo cumplido). Un burnout **no** es un error del programa:
es un final válido de la simulación.

---

# Fase 10: pulir y demostrarlo contra el subject

## 10.1 Startup stagger

**Cómo se planteó la task.** Al arrancar, todos los coders piden dongles a la
vez y tienden a encadenarse en orden de asiento: compila el 1, luego el 2...
Si los asientos pares esperan un instante al principio, los impares cogen sus
dongles primero y varios coders que no son vecinos compilan **en paralelo**
desde el inicio.

### 10.1.1 Sleep start turn

#### `sleep_start_turn`

```c
int	sleep_start_turn(t_coder *coder)
```

Si el `id` es impar, vuelve enseguida (`0`). Si es par, duerme
`START_TURN_MS` (1 ms) con `sleep_ms`. Devuelve lo mismo que `sleep_ms`.

Es una heurística: no cambia quién gana una petición con fifo o edf, solo
cuándo entran los pares. Si el sistema retrasa un hilo impar, simplemente se
vuelve al orden en serie, que sigue siendo seguro.

### 10.1.2 Run coder start turn

`run_coder` llama a `sleep_start_turn` una vez antes de su bucle.

## 10.2 Subject verification

**Cómo se planteó la task.** Con el programa completo, tocaba demostrar que
cumple el subject probando el **binario de verdad**, no funciones sueltas.

### 10.2.1 Viability boundary

Tests que ejecutan el binario con `edf` justo en la frontera de viabilidad
(y por encima, incluido un caso de 6 coders) y comprueban que nadie se quema.

> [!question] Pregunta de defensa: "¿Cuándo garantizas que con edf no se quema nadie?"
> Con `C` = compilar, `D` = cooldown, `R` = depurar + refactorizar,
> `B` = burnout y `N` = coders, una condición **suficiente** es:
>
> `N >= 2` y `B >= C + R + N·(C + D) + 10`
>
> Idea: un coder que pide dongles tiene, como mucho, `N - 1` peticiones por
> delante, y cada una tarda como máximo `C + D` (lo que tarda en acabar la
> compilación en curso más el cooldown). Los 10 ms son margen para el
> sistema operativo.
>
> Hay que ser honesto con lo que es: es un **argumento bajo supuestos**, no
> una garantía absoluta. Supone que el sistema no retrasa un hilo más que
> ese margen y que imprimir o despertar no tarda más. Si el sistema retrasa
> a un coder que tiene los dongles, el tiempo de servicio real puede superar
> `C + D`. Lo que está **probado** son los escenarios concretos de los tests
> (en la frontera y por encima, incluido uno de 6 coders), que no se queman.
> Muchos casos por debajo de la cota también sobreviven, pero esos no los
> reclamo como garantizados. Con un solo coder nunca es
> viable (solo hay un dongle). Si `C + R > B`, tampoco: el ciclo es más
> largo que el plazo.

### 10.2.2 Observable rules

Tests que leen el log completo de una ejecución y comprueban reglas
observables: dos `has taken a dongle` antes de cada `is compiling`, el ciclo
compilar → depurar → refactorizar, que dos vecinos nunca compilan a la vez,
que se respeta el cooldown, que en el caso de 2 coders se alternan y que no
sale nada después de `burned out`.

### 10.2.3 Literal errors with write

**Cómo se planteó.** Al revisar los símbolos del binario se vio que el
compilador convierte un `fprintf(stderr, "texto fijo")` en una llamada a
`fwrite`, que **no** está en la lista de funciones permitidas. Por eso los
mensajes fijos (`print_usage`, `print_run_error` y el error del scheduler)
pasaron a usar `write(2, texto, strlen(texto))`. El mensaje de los argumentos
numéricos tiene formato (`%s`, `%d`), así que sigue siendo `fprintf`, que sí
está permitida.

### 10.2.4 Evaluation runs

Las pruebas típicas de una evaluación: que el binario solo usa funciones
permitidas, que no hay variables globales, un caso imposible que tiene que
quemarse, la salida de una entrada inválida, valgrind sin fugas y
ThreadSanitizer sin data races en varios escenarios.

## 10.3 README

### 10.3.1 Complete README

Se completó `README.md` con las secciones que pide el subject (instrucciones,
recursos, uso de la IA, casos de bloqueo, mecanismos de sincronización)
usando salidas reales del programa. Codex pidió en la revisión corregir dos
cosas para que coincidieran con el código: la explicación del deadlock
(romper *hold and wait* cogiendo los dos dongles a la vez, y la espera
circular con el orden de los mutex) y que el burnout ocurre al **alcanzar**
el plazo.

---

# Issues: lo que cambió después

Los issues están integrados en cada función arriba. Aquí tienes el resumen
para localizarlos rápido:

| Issue | Qué cambió | Dónde está explicado |
|---|---|---|
| ISS-14 | `parse_config` usa el array `invalid[9]` en vez de una máscara de bits | [[#2.3.3 Parse config]] |
| ISS-32 | `sleep_ms` devuelve `0` / `1` (parada) / `-1` (error) | [[#3.3.1 Sleep ms]] |
| ISS-34 | `print_state` devuelve `0` / `1` (parada) / `-1` (error) | [[#3.4.3 Print state]] |
| ISS-46.1 | `set_compile_start` no borra un plazo ya vencido | [[#7.1.1 Set compile start]] |
| ISS-46.2 | El monitor comprueba el burnout antes que el objetivo | [[#8.2.1 Run monitor]] |

# Chuleta para la defensa

- **Deadlock:** de las cuatro condiciones de Coffman, rompemos *hold and
  wait* (los dos dongles a la vez o ninguno) y la espera circular (mutex en
  orden de índice). La exclusión mutua y la no apropiación se mantienen, que
  es lo que pide el problema.
- **Inanición:** el orden `(key, ticket)` es total; el que más prioridad
  tiene siempre es la cabeza de sus dos heaps y acaba compilando.
- **Data races:** cada dato compartido tiene su mutex: `stopped` y `failed`
  con `stop_mutex`, el log con `log_mutex`, los tickets con `ticket_mutex`,
  cada dongle con el suyo y cada coder con el suyo.
- **Logs:** un `write` por línea con `log_mutex`; se mira `stopped` dentro
  del mismo mutex, así que nada sale después del final salvo `burned out`.
- **Burnout en ≤ 10 ms:** el monitor mira cada ~1 ms y escribe sin buffer.
  Deja mucho margen y los tests lo comprueban, pero depende de que el sistema
  no retrase el hilo; no es una garantía matemática.
- **Sin busy-wait:** se espera con `pthread_cond_timedwait`; los sleeps van a
  trozos de 500 µs para enterarse pronto de la parada.
- **Cooldown:** `released_at` se guarda al soltar; `is_dongle_ready` exige
  que haya pasado `dongle_cooldown`.
- **Un coder:** coge su único dongle, no puede compilar nunca y se quema a
  los `time_to_burnout` ms.
- **0 compilaciones:** sale con `0` al momento, sin hilos ni salida.
