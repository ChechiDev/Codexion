*This project has been created as part of the 42 curriculum by sperez-l.*

# Codexion

## Description

Codexion is a small concurrency exercise from the 42 curriculum, written in C
with POSIX threads. Several "coders" sit around a table and share a limited
number of USB dongles. A coder needs two dongles to compile, then debugs,
then refactors, and so on, while a monitor watches that nobody burns out.

The interesting part is not the story but the problem underneath it: many
threads compete for few resources, and the program must stay correct and never
get stuck. That is what this project is about.

The program prints one line per event, in the form `<timestamp_ms> <coder_id>
<event>`, and a run is driven by eight command-line arguments that are
explained below.

## Instructions

The whole deliverable lives in the `coders/` directory, and it is built with
its own Makefile. From the repository root, run:

```
make -C coders
```

If you prefer, you can do the same from inside the directory:

```
cd coders
make
```

Either way you get an executable called `codexion` inside `coders/`. The
compiler is `cc` with `-Wall -Wextra -Werror -pthread`, so any warning stops
the build, and the threads library is linked.

The Makefile has the usual rules:

- `all` (the default) builds `codexion`. `make codexion` does the same.
- `clean` removes the object files (`.o`) and keeps the executable.
- `fclean` does what `clean` does and also removes the executable.
- `re` is `fclean` followed by `all`: a rebuild from scratch.

Running `make -C coders` twice in a row does not relink. The executable depends
only on the object files, and each object depends on its source file, so when
nothing changed `make` has nothing to do and says so. Relinking on every call
would waste time and is not allowed by the subject.

### Running it

The program takes exactly eight arguments, all of them required:

```
./coders/codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Here is what each one means, in plain words:

- `number_of_coders`: how many coders sit at the table. There are as many
  dongles as coders.
- `time_to_burnout`: how long, in milliseconds, a coder can go without starting
  a compile before burning out. Burning out ends the simulation.
- `time_to_compile`: how long, in milliseconds, a coder spends compiling while
  holding two dongles.
- `time_to_debug`: how long, in milliseconds, a coder spends debugging after a
  compile.
- `time_to_refactor`: how long, in milliseconds, a coder spends refactoring
  after debugging, before trying to compile again.
- `number_of_compiles_required`: how many times each coder has to compile for
  the simulation to be considered a success.
- `dongle_cooldown`: how long, in milliseconds, a dongle rests after being
  released before anyone can take it again.
- `scheduler`: the rule used to decide who gets a dongle first when several
  coders want it. It is either `fifo` (first come, first served) or `edf`
  (earliest deadline first, so the coder closest to burning out goes first).

If the number of arguments is wrong, the program prints a short reminder to the
error output and stops with exit status 1. For example, running it with no
arguments at all:

```
$ ./coders/codexion

Usage: ./codexion coders burnout compile debug refactor compiles cooldown fifo|edf

  e.g. ./codexion 4 800 200 200 200 3 50 edf
```

Argument values are validated before the program does any simulation work.
Numeric arguments must contain decimal digits only, fit in the range up to
2147483647, and may have surrounding whitespace; the number of coders and time
to burnout must be at least 1, while the other numeric values may be 0. The
scheduler must be exactly `fifo` or `edf`. Every invalid value gets its own
error line, followed by the usage reminder, and the program exits with status
1. For example:

```
$ ./coders/codexion 4 -5 abc 200 200 3 50 rr
Error: time_to_burnout: expected an integer from 1 to 2147483647
Error: time_to_compile: expected an integer from 0 to 2147483647
Error: scheduler: expected fifo or edf

Usage: ./codexion coders burnout compile debug refactor compiles cooldown fifo|edf

  e.g. ./codexion 4 800 200 200 200 3 50 edf
```

A normal simulation ends with status 0, whether each coder reached the compile
target or a coder burned out. Requiring zero compiles also exits successfully
without starting the simulation. If an internal runtime operation fails, the
program exits with status 2 and prints `Error: simulation failed` to stderr.
Simulation events are written to stdout.

### Example runs

Every output below was copied from a real run of the final binary. Timestamps
are in milliseconds since the simulation started, and the exact numbers can
shift by a millisecond or two between runs.

**A normal end by compile count.** Two coders, a generous burnout limit, and
each coder must compile once:

```
$ ./coders/codexion 2 1010 200 200 200 1 0 edf
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
200 2 has taken a dongle
200 2 has taken a dongle
200 2 is compiling
400 1 is refactoring
400 2 is debugging
```

Coder 1 takes both dongles, compiles for 200 ms and moves on to debug, which
frees the dongles. Coder 2 gets them right away and starts compiling at 200 ms.
When both have compiled once, the monitor stops the run and the exit status is
0.

**A burnout.** The same table, but the burnout limit is only 400 ms and each
coder must compile twice:

```
$ ./coders/codexion 2 400 200 200 200 2 50 fifo
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
250 2 has taken a dongle
250 2 has taken a dongle
250 2 is compiling
400 1 is refactoring
400 1 burned out
```

Coder 1 started compiling at 0 and had not started again when 400 ms were
reached, so it burned out at exactly that moment. The monitor prints that line last and nothing is printed after it.

**fifo and edf.** With `fifo` the dongle goes to whoever asked first. With `edf`
it goes to the coder whose deadline (last compile start plus `time_to_burnout`)
is closest. On a table with enough room, both give the same result:

```
$ ./coders/codexion 3 800 100 100 100 1 20 fifo
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
100 1 is debugging
120 3 has taken a dongle
120 3 has taken a dongle
120 3 is compiling
200 1 is refactoring
220 3 is debugging
240 2 has taken a dongle
240 2 has taken a dongle
240 2 is compiling
320 3 is refactoring
340 2 is debugging
```

Running the same line with `edf` printed an identical trace here. The
difference only shows up when the table is tight, because `edf` then serves the
most urgent coder first.

### When is a run viable under edf?

The subject asks that nobody burns out under `edf` when the parameters are
viable. This project uses a sufficient condition. With `N` coders,
`C = time_to_compile`, `D = dongle_cooldown`, `R = time_to_debug +
time_to_refactor` and `B = time_to_burnout`, a run is considered viable when:

```
N >= 2 and B >= C + R + N * (C + D) + 10
```

The 10 ms is slack for the operating system. This is a bound that is enough,
not the definition of every run that survives, and a single coder (one dongle)
can never be viable. For example, `./coders/codexion 4 1610 200 200 200 3 50
edf` sits exactly on the bound. A run whose own cycle `C + R` is longer than
`B` can never survive.

### Start-up offset

At start-up, odd-numbered coder seats begin their first dongle request
immediately, while even-numbered seats wait once for 1 ms. This offset gives
non-neighbouring coders a chance to request first and begin compiling in
parallel. It is a scheduling heuristic, not a guarantee: the operating system
may delay a thread long enough to change the order. It does not change FIFO or
EDF arbitration after requests arrive. The offset is set by
`START_TURN_MS` in `coders/main.h`.

## Resources

Classic references used while learning the topic:

- The POSIX Threads manual pages: `pthread_create(3)`, `pthread_mutex_lock(3)`,
  `pthread_cond_wait(3)`.
- The Dining Philosophers problem (Dijkstra), the classic ancestor of this
  exercise.
- GNU Make manual, for the Makefile rules above.
- The 42 Norm, checked with `norminette`.

**How AI was used.** I used this project to learn concurrency, and AI
assistants were part of how I worked and learned. They helped break the work
into small steps, drafted code and tests, and reviewed changes. I made the
design decisions, approved each step, and used the process to study how
threads, mutexes and scheduling work.

In practice that help touched every part of the project: the development
plan, the C sources in `coders/` (parsing, dongles and their queues, the coder
and monitor threads, logging and cleanup), the unit and acceptance tests, the
Makefile and this README.

## Blocking cases handled

Here is what could go wrong when many threads fight over few dongles, and what
the program does about each case.

**Deadlock and the Coffman conditions.** A deadlock needs four things at once
(the Coffman conditions): mutual exclusion, hold and wait, no preemption, and a
circular wait. A dongle is exclusive by nature (mutual exclusion stays) and
nobody can snatch one from a coder (no preemption stays), so the program
removes the other two conditions with two separate mechanisms.

The first removes hold and wait. A coder never holds one dongle while waiting
for the other. It queues for both, and only marks them as taken, together in
one step, when it is at the head of both queues and both are off cooldown.
Otherwise it takes nothing and waits.

The second removes the circular wait at the mutex level. The short critical
sections that inspect and update the two dongles always lock the mutexes in the
same global order, the lower dongle index first, so a ring of coders each
locking one mutex and waiting for the next cannot form.

**Starvation.** Each dongle keeps a priority queue (a heap) of the coders
waiting for it. Under `fifo` the key is a ticket number taken when the request
is made, so nobody can be overtaken forever. Under `edf` the key is the coder's
burnout deadline, so the coder closest to burning out is served first and its
wait is bounded when the parameters are viable.

**Dongle cooldown.** When a coder releases its dongles, they record the release
time. A dongle only counts as ready for the next coder once `dongle_cooldown`
milliseconds have passed. A waiting coder sleeps on a condition variable with a
short timeout and re-checks, so it neither spins nor misses the moment the
cooldown ends.

**Burnout detection.** The monitor thread scans the coders about once per
millisecond. A coder has burned out as soon as `time_to_burnout`
milliseconds have elapsed since its last compile start (reaching the limit is
enough, it does not have to be exceeded). The monitor then prints
`burned out` and stops the run, and the line appears within 10 ms of the real
burnout. It also stops the run once every coder reached the compile target.

**Log serialization.** Without care, two threads writing at once could
interleave their lines. Every event is built in a buffer and sent with a single
`write` while holding the log mutex, so each line comes out whole. The
burnout line is printed under that same mutex together with the stop flag, so no
coder event can appear after it.

## Thread synchronization mechanisms

The program uses only `pthread_mutex_t` and `pthread_cond_t`. Each one protects
one clearly named piece of shared data.

- **Dongles.** Every dongle has its own `pthread_mutex_t` and its own
  `pthread_cond_t`. The mutex protects the dongle's `available` flag, its
  `released_at` time and its heap of waiting requests. A coder that is not
  first in line waits on the condition variable of its first dongle. Releasing
  the dongles marks them as available and broadcasts on both condition
  variables so the waiting neighbours re-check.
- **Logging.** One `log_mutex` serializes every line written to stdout. The
  timestamp is computed inside the critical section, so lines are also printed
  in non-decreasing time order.
- **Monitor state.** The stop flag lives behind `stop_mutex`; every thread reads
  it through `is_stopped` and the monitor sets it through `set_stopped`. Each
  coder also has its own mutex protecting its `last_compile_start` and
  `compile_count`, which the monitor reads to decide whether the coder burned
  out or reached the target. A small `ticket_mutex` hands out the FIFO tickets.

**How races are avoided.** No shared field is touched without its mutex. For
instance, a coder updates `last_compile_start` under its own coder mutex, and
the monitor reads it under that same mutex, so the monitor never sees a
half-written value. When a lock order matters (the log mutex before the stop
mutex, the lower dongle before the higher one) it is always the same.

**How coders and the monitor talk safely.** They never call each other. The
coders publish their progress through the per-coder mutex, and the monitor
publishes the end of the run through the stop flag. When the monitor decides to
stop, it sets the flag and then locks each dongle mutex and broadcasts on its
condition variable. Coders waiting for a dongle wake up, see the flag and
leave, so no thread stays blocked and every thread can be joined.
