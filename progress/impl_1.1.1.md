# Implementation 1.1.1 - Deliverable Makefile

## Files
- `coders/Makefile` (new): NAME/CC/CFLAGS (`-Wall -Wextra -Werror -pthread -I.`), explicit `SRC = main.c`, `OBJ = $(SRC:.c=.o)`, rules all, $(NAME), %.o: %.c, clean, fclean, re, `.PHONY`. `$(NAME)` depends only on `$(OBJ)`, so no relink.
- `coders/main.c` (new): `int main(void)` returning 0.
- `README.md` (new): italic first line, Description, Instructions (build only, clean/fclean/re, no-relink explanation), Resources (+ AI note), Blocking cases handled and Thread synchronization mechanisms (placeholders). No emojis.

## Decisions
- 42 headers generated with the official vim plugin on scratch copies (same file names), pasted with Write; followed by one empty line.
- No header dependency (comes in 1.2.1). Build artifacts (`coders/codexion`, `coders/main.o`) are left built, as the acceptance test requires.

## Outputs
`norminette coders/`:
```
main.c: OK!
```
`make verify`:
```
[OK]    norminette
[SKIP]  cppcheck not installed
make[1]: Nothing to be done for 'all'.
[OK]    compiles clean (-Wall -Wextra -Werror -pthread), no relink
[OK]    tests passed
```
`make check` (tail): all sections `[OK]`, ending with `[OK] Harness invariants OK.`
