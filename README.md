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

At this stage the repository only contains the build skeleton. The simulation
itself is added step by step, and this README grows with it.

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
Usage: ./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
  scheduler is either fifo or edf
```

For now that is all the program does: it only checks how many arguments you
gave. Checking the values themselves and running the simulation are added in
later steps, and this section will grow with them.

## Resources

Classic references used while learning the topic:

- The POSIX Threads manual pages: `pthread_create(3)`, `pthread_mutex_lock(3)`,
  `pthread_cond_wait(3)`.
- The Dining Philosophers problem (Dijkstra), the classic ancestor of this
  exercise.
- GNU Make manual, for the Makefile rules above.
- The 42 Norm, checked with `norminette`.

How AI was used: an AI assistant helps with planning, reviewing and
implementing parts of the code. Nothing is accepted blindly. The author reads,
tests and understands every line that ends up in the repository.

## Blocking cases handled

There is no concurrency yet, so there is nothing to block. This section will
explain, with examples, how the program avoids deadlock, starvation and
dongle cooldown problems once the simulation exists.

## Thread synchronization mechanisms

This section will describe which mutexes and condition variables protect the
shared data, how data races are avoided, and how the coders and the monitor
communicate in a thread-safe way. It is filled in as those pieces are built.
