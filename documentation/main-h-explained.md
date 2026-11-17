# `main.h` explicado línea a línea

> Este documento explica `coders/main.h`, el fichero que contiene **todas** las
> macros, estructuras y prototipos de funciones de Codexion. Está pensado para
> alguien que **no sabe programar**: cada idea se explica desde cero y con
> ejemplos de la vida diaria. Cuando hace falta, se mira dentro de los `.c`
> para contar qué hace de verdad cada función y en qué momento se usa.

---

## 1. Conceptos antes de empezar

### 1.1. Qué es un programa

Un programa es una receta que el ordenador sigue paso a paso. La receta de
Codexion empieza en una función llamada `main` (la "primera página" de la
receta) y termina cuando `main` devuelve un número: `0` si todo fue bien, otro
número si algo falló.

### 1.1.1. Qué es una función

Una función es un "mini-ayudante" con nombre. Le das unos datos (los
**argumentos**), hace un trabajo y te devuelve una respuesta (el **valor de
retorno**). Como pedirle a alguien "súmame estos dos números": le das dos
números y te devuelve uno.

En Codexion casi todas las funciones devuelven un `int` que funciona como un
semáforo: `0` = bien, `-1` = error, y a veces `1` = "paramos, pero no es un
error" (por ejemplo, la simulación ya se acabó).

### 1.1.2. Qué es un fichero `.h`

Un `.h` (*header*, cabecera) es como el **índice de un libro**: no tiene las
recetas completas, solo dice "existe una receta llamada X que recibe esto y
devuelve aquello". Las recetas completas están en los `.c`. Todos los `.c`
incluyen `main.h` para conocerse entre sí.

### 1.2. El problema que simula Codexion

Hay N **coders** sentados en una mesa redonda. Entre cada dos coders hay un
**dongle** USB (una llave). Para **compilar**, un coder necesita los **dos**
dongles que tiene a los lados. Después **depura** (*debug*) y **refactoriza**
(*refactor*), y vuelta a empezar. Si un coder pasa demasiado tiempo sin empezar
a compilar, se **quema** (*burnout*) y la simulación termina.

Es el clásico problema de "los filósofos comensales" con otro disfraz: en vez
de filósofos y tenedores, programadores y llaves USB.

### 1.3. Qué es un hilo (*thread*)

Imagina una cocina con **varios cocineros trabajando a la vez** sobre la misma
receta. Cada cocinero es un **hilo**: una línea de trabajo que avanza por su
cuenta, en paralelo con las demás.

En Codexion:

- cada coder es un hilo (N hilos);
- hay un hilo extra, el **monitor**, que vigila a todos;
- y el hilo principal (`main`), que lo arranca todo y espera al final.

### 1.4. Qué es un mutex

Un **mutex** es el **pestillo del baño**. Si está echado, nadie más puede
entrar hasta que el que está dentro lo quite. En programación sirve para que
**solo un hilo a la vez** toque un dato compartido.

- `pthread_mutex_lock` = echar el pestillo (si ya está echado, esperas en la
  puerta).
- `pthread_mutex_unlock` = quitar el pestillo.

### 1.5. Qué es una variable de condición

Una **variable de condición** (`pthread_cond_t`) es el **timbre de una sala de
espera**. Un hilo dice "me siento a esperar hasta que alguien toque el timbre"
y se duerme sin gastar energía. Cuando otro hilo cambia algo importante (por
ejemplo, deja un dongle libre), "toca el timbre" (`broadcast`) y todos los que
esperaban se despiertan a mirar si ya les toca.

Siempre va de la mano de un mutex: para sentarte en la sala de espera tienes
que tener el pestillo; al dormirte lo sueltas automáticamente, y al despertarte
lo recuperas.

En Codexion la espera es **con despertador**: `pthread_cond_timedwait` espera
como mucho 1 ms. Así, aunque nadie toque el timbre, el coder se despierta solo
y vuelve a mirar.

### 1.6. Qué es una condición de carrera (*race condition*)

Pasa cuando dos hilos tocan el mismo dato a la vez y el resultado depende de
quién llega primero. Ejemplo cotidiano: dos personas miran que queda un yogur
en la nevera, las dos piensan "me lo como" y las dos van a por él. Resultado
roto. Los mutex evitan esto: "mirar y coger" se hace con el pestillo echado.

### 1.7. Qué es un interbloqueo (*deadlock*)

Dos personas en un pasillo estrecho, cada una esperando a que la otra se
aparte. Nadie se mueve nunca. En la mesa: cada coder coge el dongle de su
izquierda y espera el de la derecha… que tiene el vecino, que espera el suyo…
Todos esperan para siempre.

Codexion lo evita así: cada coder bloquea **siempre primero el dongle de número
más bajo** y después el más alto (lo prepara `get_dongle_pair`). Con un orden
común para todos, el círculo de esperas no puede cerrarse.

### 1.8. El tiempo en milisegundos

Todo se mide en **milisegundos (ms)**: 1000 ms = 1 segundo. El programa
apunta la hora de arranque (`start_ms`) y todos los mensajes muestran "cuántos
ms han pasado desde que empezó". Por eso los logs empiezan con números como
`0`, `200`, `401`…

### 1.9. Qué es un *heap* (cola de prioridad)

Imagina la **cola de urgencias de un hospital**: no entra primero el que llegó
antes, sino el más urgente. Un *heap* es una estructura que siempre te deja el
elemento "más prioritario" arriba del todo, listo para sacarlo rápido.

Cada dongle tiene su propio heap con las **peticiones** de los coders que lo
quieren. Solo el que está **en la cabeza** de la cola puede llevárselo.

### 1.10. FIFO frente a EDF

Son las dos formas de decidir quién va primero en la cola de un dongle:

- **FIFO** (*First In, First Out*): como la cola del supermercado. El que pidió
  antes, pasa antes. Cada petición coge un **ticket** numerado (como en la
  carnicería) y gana el número más bajo.
- **EDF** (*Earliest Deadline First*): como urgencias. Pasa primero el que
  **antes se va a quemar**. Su "fecha límite" es `último inicio de compilación
  + time_to_burnout`. Si dos tienen la misma fecha, desempata el ticket.

### 1.11. Cooldown

Después de soltar un dongle, este necesita "enfriarse" un rato
(`dongle_cooldown` ms) antes de que otro lo pueda coger. Como la plancha del
pelo que hay que dejar reposar antes de volver a usarla.

### 1.12. Burnout

Si un coder pasa `time_to_burnout` ms sin **empezar** una compilación (contando
desde el inicio o desde su última compilación), se quema. Se imprime
`<ms> <coder> burned out` y la simulación termina.

### 1.13. El monitor

Es el **socorrista de la piscina**: no se baña, solo vigila. Cada milisegundo
repasa a todos los coders y comprueba dos cosas: ¿alguien se ha quemado?
¿Todos han compilado ya las veces que tocaba? Si pasa cualquiera de las dos,
para la simulación.

---

## 2. Includes y protección de la cabecera

### 2.1. `#ifndef MAIN_H` / `# define MAIN_H` / `#endif`

Es un "esto ya lo he leído". Muchos `.c` incluyen `main.h`; si por lo que sea
se incluyera dos veces en el mismo fichero, el compilador vería todo repetido y
se quejaría. La primera vez se define `MAIN_H`; la segunda, como ya existe, se
salta todo hasta `#endif`.

### 2.2. `# include <pthread.h>`

Es el **único** include de la cabecera. Trae la "caja de herramientas" de los
hilos POSIX:

- los tipos `pthread_t` (un hilo), `pthread_mutex_t` (un pestillo) y
  `pthread_cond_t` (un timbre), que aparecen dentro de las estructuras;
- `struct timespec` (una hora exacta con segundos y nanosegundos), que usa el
  prototipo de `get_deadline`.

El resto de includes (`<stdlib.h>`, `<unistd.h>`, `<sys/time.h>`, etc.) están
en cada `.c` que los necesita, para que la cabecera cargue solo lo
imprescindible.

---

## 3. Macros

Una macro (`#define NOMBRE valor`) es una **etiqueta con un valor fijo**. El
compilador sustituye el nombre por el valor antes de compilar. Sirve para no
tener "números mágicos" sueltos por el código.

### 3.1. `EXIT_USAGE 1`

Código de salida cuando **el usuario se ha equivocado al lanzar el programa**
(número de argumentos incorrecto o algún valor no válido). Lo devuelve `main`.

### 3.2. `EXIT_RUNTIME 2`

Código de salida cuando los argumentos eran buenos pero **la simulación falló
por dentro** (no se pudo reservar memoria, crear un hilo, etc.). Lo devuelve
`main`.

### 3.3. `SLEEP_SLICE_US 500`

Tamaño máximo, en **microsegundos** (500 µs = medio milisegundo), de cada
"cabezadita" de `sleep_ms`. En vez de dormir 200 ms de golpe, el coder duerme a
trocitos de medio milisegundo y entre trozo y trozo mira si la simulación se ha
parado. Así reacciona rápido.

### 3.4. `NUMBER_TEXT_MAX 20`

Tamaño máximo, en caracteres, de un número escrito como texto. Un `long` de 64
bits tiene como mucho 19 cifras; 20 deja margen. Lo usa `build_number_text`.

### 3.5. `LOG_TEXT_MAX 32`

Longitud máxima de la parte de texto de un mensaje de log (por ejemplo
`"has taken a dongle"`). `build_log_line` rechaza textos más largos.

### 3.6. `LOG_LINE_MAX 64`

Tamaño de la "hoja" donde se escribe una línea de log completa
(`<ms> <coder> <texto>\n`). `print_log_line` reserva un hueco de este tamaño.

### 3.7. `START_TURN_MS 1`

Retraso, en ms, que esperan los coders **pares** antes de empezar. Así los
impares cogen dongles primero y los vecinos no se pisan todos en el mismo
instante. Lo usa `sleep_start_turn`.

---

## 4. Tipos: enums y structs

Un **enum** es una lista cerrada de opciones con nombre (como los botones de
un ascensor). Un **struct** es una **ficha** con varios campos, como la ficha
de un paciente: nombre, edad, alergias…

`typedef` solo le pone un nombre corto al tipo: en vez de escribir
`struct s_coder`, se escribe `t_coder`.

### 4.1. `t_state` (enum)

Lo que un coder puede anunciar por pantalla:

| Valor | Texto que se imprime |
|---|---|
| `STATE_TAKEN_DONGLE` | `has taken a dongle` |
| `STATE_COMPILING` | `is compiling` |
| `STATE_DEBUGGING` | `is debugging` |
| `STATE_REFACTORING` | `is refactoring` |

Lo recibe `print_state`, que lo traduce a texto. (`burned out` no está aquí
porque lo imprime el monitor con su propia función, `print_burnout`.)

### 4.2. `t_mode` (enum)

La política de la cola: `MODE_FIFO` o `MODE_EDF` (ver 1.10). Se elige con el
último argumento del programa (`fifo` o `edf`).

### 4.3. `t_config` (struct): la configuración

Es la **hoja de instrucciones** que escribe el usuario al lanzar el programa.
Se rellena una vez en `parse_config` y luego nadie la cambia.

#### 4.3.1. `number_of_coders`
Cuántos coders hay en la mesa (y, por tanto, cuántos dongles). Mínimo 1.

#### 4.3.2. `time_to_burnout`
Cuántos ms puede estar un coder sin empezar a compilar antes de quemarse.
Mínimo 1.

#### 4.3.3. `time_to_compile`
Cuántos ms dura compilar (con los dos dongles en la mano).

#### 4.3.4. `time_to_debug`
Cuántos ms dura depurar (ya sin dongles).

#### 4.3.5. `time_to_refactor`
Cuántos ms dura refactorizar.

#### 4.3.6. `number_of_compiles_required`
Cuántas compilaciones tiene que hacer **cada** coder para que la simulación
acabe bien. Si es `0`, la simulación ni arranca.

#### 4.3.7. `dongle_cooldown`
Cuántos ms tiene que "enfriarse" un dongle tras soltarse (ver 1.11).

#### 4.3.8. `mode`
FIFO o EDF (un `t_mode`).

### 4.4. `t_slice` (struct): un trozo de texto

Es como poner **dos dedos sobre una frase**: uno donde empieza (`start`) y la
cuenta de cuántas letras abarca (`len`). No copia el texto, solo lo señala.

#### 4.4.1. `start`
Dónde empieza el trozo.

#### 4.4.2. `len`
Cuántos caracteres tiene.

Lo devuelve `trim_slice` para quitar espacios de los lados de un argumento sin
copiarlo.

### 4.5. `t_request` (struct): una petición de dongle

Es el **papelito de la cola** que un coder deja en un dongle diciendo "lo
quiero yo".

#### 4.5.1. `key`
La prioridad. En FIFO es el ticket; en EDF es la fecha límite de burnout. Más
pequeño = pasa antes.

#### 4.5.2. `ticket`
Número de turno único y creciente (como el de la carnicería). Desempata cuando
dos `key` son iguales.

#### 4.5.3. `coder`
Quién hizo la petición (su `id`, empezando en 1).

### 4.6. `t_heap` (struct): la cola de prioridad

#### 4.6.1. `items`
La fila de papelitos (`t_request`), guardada en memoria pedida al sistema.

#### 4.6.2. `size`
Cuántos papelitos hay ahora mismo.

#### 4.6.3. `capacity`
Cuántos caben como máximo. Cada dongle lo comparten 2 coders, así que la
capacidad es 2.

### 4.7. `t_dongle` (struct): una llave USB

#### 4.7.1. `mutex`
El pestillo del dongle: para mirar o tocar este dongle hay que echarlo.

#### 4.7.2. `cond`
El timbre del dongle: quien espera por él duerme aquí; quien lo suelta, toca.

#### 4.7.3. `heap`
La cola de peticiones de este dongle.

#### 4.7.4. `available`
`1` si está libre encima de la mesa, `0` si alguien lo tiene en la mano.

#### 4.7.5. `released_at`
En qué ms se soltó por última vez (para calcular el cooldown). `-1` significa
"nunca se ha usado", así que no hay que esperar enfriamiento.

### 4.8. `t_coder` (struct): un programador

#### 4.8.1. `id`
Su número, de 1 a N (es el que se imprime en los logs).

#### 4.8.2. `first_dongle`
Índice del dongle que debe bloquear **primero** (el de número más bajo de sus
dos lados).

#### 4.8.3. `second_dongle`
Índice del segundo dongle (el más alto). Si solo hay un coder, los dos son el
mismo dongle.

#### 4.8.4. `mutex`
Pestillo propio del coder, que protege `last_compile_start` y
`compile_count`, porque el monitor los lee mientras el coder los escribe.

#### 4.8.5. `last_compile_start`
En qué ms empezó su última compilación (o 0 al principio). Es su "reloj de
burnout".

#### 4.8.6. `compile_count`
Cuántas compilaciones ha terminado.

#### 4.8.7. `thread`
El "identificador" de su hilo, para poder esperarlo al final.

#### 4.8.8. `sim`
Un puntero (una "flecha") a la simulación entera, para que el coder pueda
llegar a los dongles, la configuración, los pestillos globales, etc. Se escribe
`struct s_sim *` porque `t_sim` se define más abajo.

### 4.9. `t_sim` (struct): la simulación entera

Es la **sala completa**: la mesa, las personas, las llaves y las normas.

#### 4.9.1. `config`
Copia de la configuración del usuario.

#### 4.9.2. `start_ms`
Hora (en ms) en que arrancó la simulación. Todo se mide desde aquí.

#### 4.9.3. `stopped`
`1` cuando la simulación debe parar (alguien se quemó, todos acabaron o hubo un
error). Todos los hilos lo consultan.

#### 4.9.4. `failed`
`1` si la parada fue por un **error interno**. Hace que `main` acabe con
`EXIT_RUNTIME`.

#### 4.9.5. `monitor`
Identificador del hilo monitor.

#### 4.9.6. `next_ticket`
El siguiente número de la "máquina de tickets".

#### 4.9.7. `dongles`
La fila de N dongles.

#### 4.9.8. `coders`
La fila de N coders.

#### 4.9.9. `stop_mutex`
Pestillo que protege `stopped` y `failed`.

#### 4.9.10. `log_mutex`
Pestillo del "micrófono": solo un hilo escribe en pantalla a la vez, para que
las líneas no se mezclen.

#### 4.9.11. `ticket_mutex`
Pestillo de la máquina de tickets: dos coders nunca sacan el mismo número.

---

## 5. Funciones, una a una

Cada bloque sigue el orden de `main.h`. Para cada función: qué hace, qué recibe,
qué devuelve y dónde encaja en el programa.

### 5.1. Utils (utilidades)

#### 5.1.1. `int is_digit(char c)`
- **Qué hace:** dice si un carácter es una cifra del `0` al `9`.
- **Recibe:** `c`, un carácter.
- **Devuelve:** 1 si es cifra, 0 si no.
- **Dónde:** la usa `parse_number` letra a letra.

#### 5.1.2. `int is_space(char c)`
- **Qué hace:** dice si un carácter es "espacio en blanco" (espacio,
  tabulador, salto de línea…).
- **Recibe:** `c`.
- **Devuelve:** 1 o 0.
- **Dónde:** la usa `trim_slice`.

#### 5.1.3. `t_slice trim_slice(const char *str)`
- **Qué hace:** como recortar los márgenes de una foto: localiza el texto sin
  los espacios de delante y de detrás, sin copiarlo.
- **Recibe:** `str`, el texto.
- **Devuelve:** un `t_slice` (inicio + longitud). Si el texto no existe, longitud 0.
- **Dónde:** la llama `parse_number`, para aceptar `" 200 "` como `200`.

#### 5.1.4. `int build_number_text(long n, char *buf)`
- **Qué hace:** escribe un número como texto (`401` → `'4' '0' '1'`) en `buf`.
- **Recibe:** `n` (no negativo) y `buf`, el hueco donde escribir.
- **Devuelve:** cuántas cifras escribió, o -1 si `n < 0` o no hay hueco.
- **Dónde:** la usa `build_log_line` para el timestamp y el id del coder.

### 5.2. Parsing (leer los argumentos)

#### 5.2.1. `int get_arg_minimum(int position)`
- **Qué hace:** dice el valor mínimo permitido para cada argumento.
- **Recibe:** `position`, de 1 a 8.
- **Devuelve:** 1 para las posiciones 1–2 (coders y burnout), 0 para la 3–7, -1
  para cualquier otra.
- **Dónde:** la usa `parse_config` para validar y `print_arg_error` para
  decirle al usuario el rango correcto.

#### 5.2.2. `int parse_number(const char *str, int *out)`
- **Qué hace:** convierte un texto en un número entero, letra a letra. Rechaza
  signos, letras, texto vacío y números que no caben en un `int`.
- **Recibe:** `str` (el texto) y `out` (dónde dejar el número).
- **Devuelve:** 0 si lo consiguió (y escribe en `out`), -1 si no.
- **Dónde:** la llama `parse_config` para los 7 argumentos numéricos.

#### 5.2.3. `int parse_mode(const char *str, t_mode *out)`
- **Qué hace:** traduce `"fifo"` o `"edf"` a `MODE_FIFO` / `MODE_EDF`.
- **Recibe:** `str` y `out`.
- **Devuelve:** 0 si era una de las dos palabras, -1 si no.
- **Dónde:** la llama `parse_config` para el argumento 8.

#### 5.2.4. `int parse_config(char **argv, t_config *config, int *invalid)`
- **Qué hace:** lee los 8 argumentos y rellena la ficha `config`. No se para en
  el primer error: revisa todos y marca cuáles están mal.
- **Recibe:** `argv` (lo que escribió el usuario), `config` (la ficha a
  rellenar) e `invalid` (una lista de 9 casillas donde marca con 1 cada
  posición incorrecta).
- **Devuelve:** cuántos argumentos estaban mal (0 = todo correcto).
- **Dónde:** la llama `main`. Si devuelve algo distinto de 0, `main` imprime un
  error por cada casilla marcada, el uso, y sale con `EXIT_USAGE`.

### 5.3. Time (tiempo)

#### 5.3.1. `long get_time_ms(void)`
- **Qué hace:** mira el reloj del sistema y lo da en milisegundos.
- **Recibe:** nada.
- **Devuelve:** la hora en ms, o -1 si el reloj falla.
- **Dónde:** `start_threads` la usa para apuntar `start_ms`; `sleep_ms` y
  `get_elapsed_ms` también.

#### 5.3.2. `long get_elapsed_ms(long start_ms)`
- **Qué hace:** "¿cuánto ha pasado desde `start_ms`?". Como un cronómetro.
- **Recibe:** `start_ms`, el momento de referencia.
- **Devuelve:** los ms transcurridos (nunca negativos), o -1 si el reloj falla.
- **Dónde:** en casi todas partes: timestamps de los logs, burnout, cooldown,
  `last_compile_start`, `sleep_ms`.

#### 5.3.3. `int get_deadline(long ms, struct timespec *out)`
- **Qué hace:** calcula la hora exacta "de aquí a `ms` milisegundos", en el
  formato que pide el despertador de `pthread_cond_timedwait`.
- **Recibe:** `ms` y `out` (dónde escribir esa hora).
- **Devuelve:** 0 o -1.
- **Dónde:** `wait_for_dongles` y la espera de un único dongle la usan con
  `ms = 1` para no dormir más de 1 ms seguido.

#### 5.3.4. `int sleep_ms(t_sim *sim, long ms)`
- **Qué hace:** duerme `ms` milisegundos, pero a trocitos de como mucho
  `SLEEP_SLICE_US`, y entre trocito y trocito mira si la simulación ha parado.
  Como echarte la siesta con un ojo abierto.
- **Recibe:** `sim` y `ms`.
- **Devuelve:** 0 si durmió todo, 1 si se interrumpió porque la simulación
  paró, -1 si falló el reloj.
- **Dónde:** el coder la usa para compilar, depurar, refactorizar, el
  arranque escalonado y la espera del coder solitario.

### 5.4. State (estado compartido)

#### 5.4.1. `int is_stopped(t_sim *sim)`
- **Qué hace:** mira (con `stop_mutex`) si la simulación está parada.
- **Recibe:** `sim`.
- **Devuelve:** 1 si está parada (o si falla el pestillo, por prudencia), 0 si no.
- **Dónde:** `sleep_ms`, el monitor en cada vuelta, y los coders mientras
  esperan dongles.

#### 5.4.2. `int set_failed(t_sim *sim)`
- **Qué hace:** apunta `failed = 1` ("ha habido un error interno").
- **Recibe:** `sim`.
- **Devuelve:** 0 o -1.
- **Dónde:** cuando un coder, el monitor o el arranque detectan un error.

#### 5.4.3. `int set_stopped(t_sim *sim)`
- **Qué hace:** apunta `stopped = 1`. Echa primero `log_mutex` y luego
  `stop_mutex`: así nadie puede estar a mitad de imprimir una línea cuando se
  da la orden de parar, y tras la parada no sale ninguna línea más.
- **Recibe:** `sim`.
- **Devuelve:** 0 o -1.
- **Dónde:** el monitor cuando todos han terminado; coders y arranque cuando
  hay un error.

#### 5.4.4. `int take_ticket(t_sim *sim, long *out)`
- **Qué hace:** saca el siguiente número de la máquina de tickets.
- **Recibe:** `sim` y `out` (dónde dejar el número).
- **Devuelve:** 0 o -1 (si la máquina se quedara sin números, cosa imposible en
  la práctica).
- **Dónde:** la llama `build_request` cada vez que un coder pide dongles.

### 5.5. Log (mensajes por pantalla)

#### 5.5.1. `int build_log_line(char *line, long ts, int coder, const char *text)`
- **Qué hace:** monta en `line` el texto `"<ts> <coder> <text>\n"`, por ejemplo
  `"200 3 is compiling\n"`. Solo lo monta, no lo imprime.
- **Recibe:** `line` (la hoja, de `LOG_LINE_MAX`), `ts` (ms), `coder` (id) y
  `text`.
- **Devuelve:** la longitud de la línea, o -1 si algún dato no es válido.
- **Dónde:** la llama `print_log_line`.

#### 5.5.2. `int print_state(t_sim *sim, int coder, t_state state)`
- **Qué hace:** anuncia por pantalla lo que hace un coder. Coge el micrófono
  (`log_mutex`), comprueba que la simulación no esté parada y, si no lo está,
  imprime.
- **Recibe:** `sim`, el `coder` (id) y el `state`.
- **Devuelve:** 0 si imprimió, 1 si no imprimió porque ya estaba parada, -1 si
  hubo error.
- **Dónde:** el coder la llama al coger cada dongle, al compilar, al depurar y
  al refactorizar.

#### 5.5.3. `int print_burnout(t_sim *sim, int coder)`
- **Qué hace:** con el micrófono cogido, para la simulación **y** escribe
  `"<ts> <coder> burned out"` en un solo paso. Así ninguna otra línea puede
  colarse después del burnout.
- **Recibe:** `sim` y `coder`.
- **Devuelve:** 0 si fue esta llamada la que paró y escribió; -1 si ya estaba
  parada o algo falló.
- **Dónde:** solo el monitor, cuando detecta a un coder quemado.

#### 5.5.4. `int print_log_line(t_sim *sim, int coder, const char *text)`
- **Qué hace:** calcula los ms, monta la línea con `build_log_line` y la envía
  a pantalla con una sola escritura. **Ojo:** da por hecho que quien la llama
  ya tiene el micrófono (`log_mutex`); ella no toca ningún pestillo.
- **Recibe:** `sim`, `coder`, `text`.
- **Devuelve:** 0 si se escribió la línea entera, -1 si no.
- **Dónde:** la llaman `print_state` y `print_burnout`.

### 5.6. Policy (normas de la cola)

#### 5.6.1. `int is_request_before(const t_request *a, const t_request *b)`
- **Qué hace:** el "árbitro" de la cola: ¿va `a` antes que `b`? Primero mira
  la `key`; si empatan, el ticket más bajo.
- **Recibe:** dos peticiones.
- **Devuelve:** 1 si `a` va antes, 0 si no.
- **Dónde:** el heap la usa para ordenar (`push_request`, `pop_request`).

#### 5.6.2. `long get_request_key(t_mode mode, long ticket, long last_compile_start, int time_to_burnout)`
- **Qué hace:** calcula la prioridad de una petición. FIFO: el ticket. EDF:
  `last_compile_start + time_to_burnout` (cuándo se quemaría).
- **Recibe:** el modo, el ticket, el último inicio de compilación y el tiempo
  de burnout.
- **Devuelve:** la clave, o -1 si los datos no tienen sentido.
- **Dónde:** la llama `build_request`.

#### 5.6.3. `int is_dongle_ready(const t_dongle *dongle, int coder, long now, int cooldown)`
- **Qué hace:** responde "¿puede este coder coger este dongle **ya**?". Las
  tres condiciones: que sea el primero de la cola, que el dongle esté libre y
  que haya pasado el cooldown desde que se soltó.
- **Recibe:** el dongle, el id del coder, la hora actual y el cooldown.
- **Devuelve:** 1 si sí, 0 si no.
- **Dónde:** la usan `take_dongles` y `take_single_dongle` en cada intento.

### 5.7. Heap (la cola de prioridad)

#### 5.7.1. `int init_heap(t_heap *heap, int capacity)`
- **Qué hace:** pide memoria para una cola de `capacity` papelitos y la deja vacía.
- **Recibe:** el heap y la capacidad.
- **Devuelve:** 0 o -1.
- **Dónde:** `init_dongle`, con capacidad 2.

#### 5.7.2. `void free_heap(t_heap *heap)`
- **Qué hace:** devuelve la memoria de la cola al sistema.
- **Recibe:** el heap.
- **Devuelve:** nada.
- **Dónde:** `destroy_dongle`, al limpiar.

#### 5.7.3. `int push_request(t_heap *heap, const t_request *req)`
- **Qué hace:** mete un papelito en la cola y lo hace "subir" hasta su sitio
  según la prioridad.
- **Recibe:** el heap y la petición.
- **Devuelve:** 0, o -1 si la cola está llena.
- **Dónde:** cuando un coder pide sus dongles (`take_dongles`,
  `take_single_dongle`).

#### 5.7.4. `int get_head_request(const t_heap *heap, t_request *out)`
- **Qué hace:** mira (sin sacar) quién está el primero de la cola.
- **Recibe:** el heap y `out`.
- **Devuelve:** 0, o -1 si la cola está vacía.
- **Dónde:** `is_dongle_ready`.

#### 5.7.5. `int pop_request(t_heap *heap, t_request *out)`
- **Qué hace:** saca al primero de la cola y reordena el resto.
- **Recibe:** el heap y `out`.
- **Devuelve:** 0, o -1 si estaba vacía.
- **Dónde:** cuando un coder por fin coge el dongle, quita su papelito.

### 5.8. Init (preparar la mesa)

#### 5.8.1. `int get_dongle_pair(int seat, int n, int *first, int *second)`
- **Qué hace:** dado el asiento de un coder, calcula sus dos dongles (el de su
  asiento y el siguiente, dando la vuelta al final) y los ordena: `first` el
  más bajo, `second` el más alto. Es la regla anti-deadlock (1.7).
- **Recibe:** `seat` (0..n-1), `n`, y dónde dejar los dos índices.
- **Devuelve:** 0 o -1.
- **Dónde:** `init_coder`.

#### 5.8.2. `int init_dongle(t_dongle *dongle)`
- **Qué hace:** prepara un dongle: su pestillo, su timbre, su cola (de 2), lo
  marca libre y "nunca usado" (`released_at = -1`). Si algo falla a medias,
  deshace lo que ya había hecho.
- **Recibe:** el dongle.
- **Devuelve:** 0 o -1.
- **Dónde:** `init_dongles`.

#### 5.8.3. `int init_dongles(t_dongle **out, int n)`
- **Qué hace:** pide memoria para `n` dongles e inicializa cada uno.
- **Recibe:** `out` (dónde dejar la fila) y `n`.
- **Devuelve:** 0 o -1 (y limpia lo creado si falla).
- **Dónde:** `init_simulation`.

#### 5.8.4. `int init_coder(t_coder *coder, t_sim *sim, int seat, int n)`
- **Qué hace:** prepara la ficha de un coder: id = asiento + 1, sus dongles, su
  pestillo, contadores a 0 y la flecha a la simulación.
- **Recibe:** el coder, la simulación, el asiento y `n`.
- **Devuelve:** 0 o -1.
- **Dónde:** `init_coders`.

#### 5.8.5. `int init_coders(t_coder **out, t_sim *sim, int n)`
- **Qué hace:** pide memoria para `n` coders e inicializa cada uno.
- **Recibe:** `out`, `sim`, `n`.
- **Devuelve:** 0 o -1.
- **Dónde:** `init_simulation`.

#### 5.8.6. `int init_simulation(t_sim *sim, const t_config *config)`
- **Qué hace:** monta la sala entera: copia la config, pone contadores a cero,
  crea los tres pestillos globales, los dongles y los coders.
- **Recibe:** `sim` y `config`.
- **Devuelve:** 0 o -1 (limpiando lo creado).
- **Dónde:** `run_simulation`, justo antes de lanzar los hilos.

### 5.9. Cleanup (recoger la mesa)

#### 5.9.1. `int destroy_coder(t_coder *coder)`
- **Qué hace:** destruye el pestillo del coder.
- **Recibe:** el coder. **Devuelve:** 0 o -1.
- **Dónde:** `free_coders`.

#### 5.9.2. `int destroy_dongle(t_dongle *dongle)`
- **Qué hace:** libera la cola y destruye el timbre y el pestillo del dongle.
- **Recibe:** el dongle. **Devuelve:** 0 o -1.
- **Dónde:** `free_dongles`.

#### 5.9.3. `int free_dongles(t_dongle *dongles, int count)`
- **Qué hace:** destruye los `count` primeros dongles (del último al primero) y
  libera la fila.
- **Recibe:** la fila y cuántos hay creados. **Devuelve:** 0 o -1.
- **Dónde:** `free_sim`, y `init_dongles` si falla a medias.

#### 5.9.4. `int free_coders(t_coder *coders, int count)`
- **Qué hace:** lo mismo para los coders.
- **Recibe:** la fila y cuántos. **Devuelve:** 0 o -1.
- **Dónde:** `free_sim`, y `init_coders` si falla a medias.

#### 5.9.5. `int free_sim(t_sim *sim)`
- **Qué hace:** recoge todo: coders, dongles y los tres pestillos globales.
- **Recibe:** `sim`. **Devuelve:** 0 o -1.
- **Dónde:** al final de `run_simulation` (o cuando falla la preparación).

### 5.10. Dongle (pedir, esperar y soltar llaves)

#### 5.10.1. `int build_request(t_coder *coder, t_request *out)`
- **Qué hace:** rellena el papelito de la cola: lee (con el pestillo del coder)
  su último inicio de compilación, saca ticket y calcula la `key`.
- **Recibe:** el coder y `out`.
- **Devuelve:** 0 o -1.
- **Dónde:** al principio de `take_dongles` y `take_single_dongle`.

#### 5.10.2. `int wake_dongles(t_sim *sim)`
- **Qué hace:** toca el timbre de **todos** los dongles para que cualquier coder
  que esté esperando se despierte y vea que hay que parar.
- **Recibe:** `sim`. **Devuelve:** 0 o -1.
- **Dónde:** el monitor al terminar, y cualquier hilo que pare por error.

#### 5.10.3. `int wait_for_dongles(t_coder *coder)`
- **Qué hace:** el coder tiene echados los pestillos de sus dos dongles pero
  todavía no le toca. Suelta el segundo, se sienta a esperar en el timbre del
  primero (como mucho 1 ms) y al despertar vuelve a echar el segundo.
- **Recibe:** el coder.
- **Devuelve:** 0 si volvió con los dos pestillos; -1 si falló (y en ese caso
  los suelta).
- **Dónde:** dentro del bucle de `take_dongles`.

#### 5.10.4. `int release_dongles(t_coder *coder)`
- **Qué hace:** deja los dos dongles en la mesa: los marca libres, apunta la
  hora (`released_at`, para el cooldown) y toca sus timbres.
- **Recibe:** el coder. **Devuelve:** 0 o -1.
- **Dónde:** justo después de compilar, y al salir el hilo si aún tenía dongles.

#### 5.10.5. `int take_dongles(t_coder *coder, int *taken)`
- **Qué hace:** el proceso completo de coger **dos** dongles:
  1. hace el papelito y lo deja en la cola de los dos dongles;
  2. en bucle: si la simulación paró, se va; si los dos están "listos para él"
     (`is_dongle_ready`), quita su papelito, los marca ocupados y listo; si no,
     espera (`wait_for_dongles`) y vuelve a mirar.
- **Recibe:** el coder y `taken` (dónde decir si los consiguió).
- **Devuelve:** 0 (con `taken` = 1 si los cogió, 0 si paró antes) o -1 (y
  entonces para la simulación y despierta a todos).
- **Dónde:** la llama `take_coder_dongles` cuando hay 2 o más coders.

#### 5.10.6. `int take_single_dongle(t_coder *coder, int *taken)`
- **Qué hace:** lo mismo, pero para el caso de **un solo coder**, que tiene un
  único dongle (izquierda y derecha son el mismo).
- **Recibe/Devuelve:** igual que `take_dongles`.
- **Dónde:** `take_coder_dongles` cuando `first_dongle == second_dongle`.

### 5.11. Coder (la vida de un programador)

#### 5.11.1. `int set_compile_start(t_coder *coder)`
- **Qué hace:** apunta "empiezo a compilar ahora" en `last_compile_start`,
  **pero solo si aún no se ha quemado**. Mirar y apuntar se hace con el mismo
  pestillo echado, para que el monitor no vea un estado a medias.
- **Recibe:** el coder.
- **Devuelve:** 0 si lo apuntó, 1 si ya era tarde (no toca nada), -1 si error.
- **Dónde:** en el bucle de `run_coder`, nada más coger los dongles.

#### 5.11.2. `int set_compile_count(t_coder *coder)`
- **Qué hace:** suma 1 a `compile_count` (con el pestillo del coder).
- **Recibe:** el coder. **Devuelve:** 0 o -1.
- **Dónde:** `run_coder`, al terminar de compilar.

#### 5.11.3. `int take_coder_dongles(t_coder *coder, int *taken)`
- **Qué hace:** coge los dongles (llamando a `take_dongles` o
  `take_single_dongle`) y anuncia `has taken a dongle` una vez por cada dongle
  (dos veces, o una si está solo).
- **Recibe:** el coder y `taken`.
- **Devuelve:** 0 (con `taken` 1 o 0) o -1.
- **Dónde:** al inicio de cada vuelta de `run_coder`.

#### 5.11.4. `int sleep_start_turn(t_coder *coder)`
- **Qué hace:** si el id del coder es par, espera `START_TURN_MS` antes de
  empezar; si es impar, nada.
- **Recibe:** el coder.
- **Devuelve:** lo mismo que `sleep_ms` (0, 1 o -1).
- **Dónde:** lo primero que hace `run_coder`.

#### 5.11.5. `void *run_coder(void *arg)`
- **Qué hace:** es **la vida entera de un coder**, el código que ejecuta su hilo:
  1. arranque escalonado (`sleep_start_turn`);
  2. bucle: coger dongles → apuntar inicio → `is compiling` → dormir
     `time_to_compile` → sumar compilación → soltar dongles → `is debugging` →
     dormir → `is refactoring` → dormir → otra vuelta;
  3. si se queda solo con un dongle (1 coder), espera dormido hasta que el
     monitor declare el burnout;
  4. al salir, suelta lo que tenga en la mano; si fue por error, marca
     `failed`, para y despierta a todos.
- **Recibe:** `arg`, que en realidad es su `t_coder` (los hilos solo aceptan
  un `void *`, un "paquete sin etiqueta").
- **Devuelve:** `NULL` siempre (el resultado real queda en `sim`).
- **Dónde:** `start_threads` lanza un hilo con esta función por cada coder.

### 5.12. Monitor (el socorrista)

#### 5.12.1. `int get_burned_out(t_coder *coder, int *burned)`
- **Qué hace:** comprueba (con el pestillo del coder) si han pasado
  `time_to_burnout` ms desde su último inicio de compilación.
- **Recibe:** el coder y `burned` (respuesta: 1 quemado, 0 no).
- **Devuelve:** 0 o -1.
- **Dónde:** el monitor, coder por coder en cada vuelta.

#### 5.12.2. `int get_target_met(t_sim *sim, int *met)`
- **Qué hace:** comprueba si **todos** los coders llegaron ya a
  `number_of_compiles_required`.
- **Recibe:** `sim` y `met` (1 sí, 0 no).
- **Devuelve:** 0 o -1.
- **Dónde:** el monitor, en cada vuelta, después de mirar el burnout.

#### 5.12.3. `void *run_monitor(void *arg)`
- **Qué hace:** el bucle del socorrista. En cada vuelta:
  1. si alguien ya paró la simulación, se va en silencio;
  2. si hay un coder quemado (el de índice más bajo), llama a `print_burnout`
     y termina;
  3. si todos llegaron al objetivo, termina;
  4. si no, duerme 1 ms y repite.

  Al terminar, para la simulación (si no estaba parada), despierta a todos y,
  si hubo un error, marca `failed`.
- **Recibe:** `arg`, que es el `t_sim`.
- **Devuelve:** `NULL`.
- **Dónde:** `start_threads` lo lanza el primero, antes que los coders.

### 5.13. Lifecycle (ciclo de vida)

#### 5.13.1. `int join_threads(t_sim *sim, int coders)`
- **Qué hace:** espera a que terminen los primeros `coders` hilos de coder y
  después el monitor. "Join" es como esperar en la puerta a que salgan todos
  antes de cerrar.
- **Recibe:** `sim` y cuántos hilos de coder llegaron a crearse.
- **Devuelve:** 0 o -1.
- **Dónde:** al final de `run_simulation`, o en el rescate si el arranque falla.

#### 5.13.2. `int start_threads(t_sim *sim)`
- **Qué hace:** pone el cronómetro en marcha (`start_ms`), lanza el monitor y
  luego un hilo por coder. Si falla al crear algún coder, marca error, para,
  despierta y espera a los que sí se crearon.
- **Recibe:** `sim`.
- **Devuelve:** 0 si todos arrancaron; -1 si falló (y se pudo recoger); -2 si
  ni siquiera se pudo esperar a los hilos creados.
- **Dónde:** `run_simulation`.

#### 5.13.3. `int run_simulation(t_sim *sim, const t_config *config)`
- **Qué hace:** el "director de orquesta": si se piden 0 compilaciones, acaba
  sin hacer nada; si no, prepara (`init_simulation`), arranca
  (`start_threads`), espera a todos (`join_threads`), mira si hubo fallo y
  recoge (`free_sim`).
- **Recibe:** `sim` (la sala, todavía vacía) y `config`.
- **Devuelve:** 0 si todo terminó bien (aunque fuera por burnout: eso es un
  final normal de la simulación), -1 si hubo un error interno.
- **Dónde:** la llama `main` tras validar los argumentos.

### 5.14. CLI (mensajes para el usuario)

#### 5.14.1. `void print_usage(void)`
- **Qué hace:** escribe en la salida de errores cómo se usa el programa, con un
  ejemplo (`./codexion 4 800 200 200 200 3 50 edf`).
- **Recibe/Devuelve:** nada.
- **Dónde:** `main`, cuando faltan/sobran argumentos o alguno es incorrecto.

#### 5.14.2. `void print_arg_error(int position)`
- **Qué hace:** escribe un error concreto para un argumento, por ejemplo
  `Error: time_to_burnout: expected an integer from 1 to 2147483647`, o
  `Error: scheduler: expected fifo or edf`.
- **Recibe:** la posición del argumento (1–8).
- **Devuelve:** nada.
- **Dónde:** `main`, una vez por cada argumento marcado como inválido.

#### 5.14.3. `void print_run_error(void)`
- **Qué hace:** escribe `Error: simulation failed`.
- **Recibe/Devuelve:** nada.
- **Dónde:** `main`, si `run_simulation` devuelve -1.

---

## 6. El programa entero, de principio a fin

### 6.1. Arranque y validación (`main`)

1. El usuario escribe, por ejemplo, `./codexion 4 800 200 200 200 3 50 edf`.
2. `main` comprueba que hay exactamente 8 argumentos. Si no: `print_usage` y
   sale con `EXIT_USAGE` (1).
3. `parse_config` lee los 8 valores. Si alguno está mal: un `print_arg_error`
   por cada uno, `print_usage` y `EXIT_USAGE`.
4. Con la config buena, `main` llama a `run_simulation`.

### 6.2. Preparación (`run_simulation` → `init_simulation`)

1. Si `number_of_compiles_required` es 0, no hay nada que hacer: devuelve 0.
2. `init_simulation` crea los pestillos globales, N dongles (libres, con su
   cola vacía) y N coders (cada uno con sus dos dongles ordenados).

### 6.3. Arranque de los hilos (`start_threads`)

1. Se apunta la hora de salida (`start_ms`). A partir de aquí los logs cuentan
   desde 0.
2. Se lanza el monitor (`run_monitor`).
3. Se lanza un hilo por coder (`run_coder`).

### 6.4. La vida de cada coder (`run_coder`)

1. Los pares esperan 1 ms; los impares empiezan ya.
2. **Pedir dongles:** deja un papelito en la cola de sus dos dongles. Espera
   hasta que esté el primero en ambas colas, los dos estén libres y hayan
   pasado su cooldown. Entonces los coge e imprime dos veces
   `has taken a dongle`.
3. **Compilar:** apunta la hora de inicio (resetea su reloj de burnout),
   imprime `is compiling`, duerme `time_to_compile`, suma 1 compilación y
   suelta los dongles (que empiezan a enfriarse y avisan a quien espere).
4. **Depurar y refactorizar:** imprime y duerme cada uno de los dos tiempos.
5. Vuelve al paso 2… hasta que la simulación se pare.

### 6.5. La vigilancia (`run_monitor`)

Cada milisegundo, el monitor repasa la mesa:

- ¿alguien se ha quemado? → `print_burnout` (para la simulación e imprime
  `burned out` en un solo paso) y se va;
- ¿todos han llegado al objetivo? → para la simulación y se va;
- si no, otra vuelta.

Al irse, toca el timbre de todos los dongles para que nadie se quede dormido
esperando.

### 6.6. El final

1. Los coders ven `stopped = 1` (en `sleep_ms` o mientras esperan dongles),
   sueltan lo que tengan y su hilo termina.
2. `run_simulation` espera a todos los hilos (`join_threads`), mira si hubo un
   error interno (`failed`) y libera toda la memoria (`free_sim`).
3. De vuelta en `main`:
   - todo bien (por objetivo cumplido **o** por burnout) → sale con `0`;
   - error interno → `print_run_error` y sale con `EXIT_RUNTIME` (2).

### 6.7. Resumen en una frase

`main` lee las instrucciones, `run_simulation` monta la mesa, los coders se
turnan los dongles por orden de cola (FIFO o EDF) respetando el cooldown, el
monitor vigila que nadie se queme y, cuando algo termina la partida, todos
recogen y el programa sale con el código adecuado.
