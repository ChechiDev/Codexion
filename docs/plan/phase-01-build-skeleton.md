# Phase 01 — Build skeleton
- goal: a Norm-clean `coders/` that builds `codexion` with the deliverable Makefile (cc -Wall -Wextra -Werror -pthread, NAME/all/clean/fclean/re, no relink), a `main.h` with the base types and a minimal `main.c`

## 1.1 Deliverable build
- layer: interfaces
- goal: `coders/Makefile` builds `codexion` from an explicit source list with the subject flags, the mandatory rules and no relink

## 1.2 Header and entry point
- layer: interfaces
- goal: `main.h` declares the base configuration types and `main.c` rejects a wrong argument count with a usage message
