# Makefile — C variant. Single source of truth for the commands.
# Keep the harness contract targets: lint / type / test / verify / check / plan
# (+ env-check, used by harness/check.sh). Customize freely below that.
.PHONY: lint type test verify check plan status env-check clean

CC      ?= gcc
CSTD    := -std=c11
WARN    := -Wall -Wextra -Werror -pedantic
CFLAGS  := $(CSTD) $(WARN) -Iinclude $(EXTRA_CFLAGS)
# Colored status tags (ok/skip/warn/fail), plain when not on a terminal.
STATUS  := ./harness/status.sh

SRC   := $(shell find src -name '*.c' 2>/dev/null)
OBJ   := $(SRC:.c=.o)
HDR   := $(shell find include src -name '*.h' 2>/dev/null)
TESTS := $(shell find tests -name 'test_*.c' 2>/dev/null)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Lint / static analysis — optional tools, run only when installed (degrade
# gracefully so the gate still works on a bare gcc+make box).
lint:
	@if command -v clang-format >/dev/null 2>&1; then \
	  echo "clang-format --dry-run -Werror"; clang-format --dry-run -Werror $(SRC) $(HDR) $(TESTS); \
	else $(STATUS) skip "clang-format not installed"; fi
	@if command -v cppcheck >/dev/null 2>&1; then \
	  echo "cppcheck"; cppcheck --enable=warning,style --error-exitcode=1 --quiet -Iinclude src; \
	else $(STATUS) skip "cppcheck not installed"; fi
	@if command -v clang-tidy >/dev/null 2>&1 && [ -n "$(SRC)" ]; then \
	  echo "clang-tidy"; clang-tidy $(SRC) -- $(CFLAGS); \
	else $(STATUS) skip "clang-tidy not installed (SOLID/clean-code enforced by review)"; fi

# C has no separate type checker: a strict warning-clean compile IS the gate.
type: $(OBJ)
	@$(STATUS) ok "compiles clean ($(WARN))"

# Quiet on success; a test that fails to build or run is reported by name.
test: $(OBJ)
	@if [ -z "$(TESTS)" ]; then $(STATUS) ok "no tests yet"; else \
	  for t in $(TESTS); do \
	    bin=$${t%.c}.bin; \
	    $(CC) $(CFLAGS) $$t $(OBJ) -o $$bin || { $(STATUS) fail "$$t (build)"; exit 1; }; \
	    ./$$bin || { $(STATUS) fail "$$t"; exit 1; }; \
	  done; \
	  $(STATUS) ok "tests passed"; \
	fi

verify: lint type test

plan:
	python3 harness/sync_plan.py

# The whole plan at a glance: done / in progress / pending, gates, next step.
status:
	@python3 harness/plan_status.py

# Both halves always run (-k keeps verify going past a red step), so one run
# reports every failure; the result is red if either half is red.
check:
	@rc=0; \
	$(MAKE) --no-print-directory -k verify || rc=1; \
	./harness/check.sh || rc=1; \
	exit $$rc

env-check:
	@command -v $(CC) >/dev/null 2>&1 || { $(STATUS) fail "$(CC) not found"; exit 1; }
	@command -v make  >/dev/null 2>&1 || { $(STATUS) fail "make not found"; exit 1; }
	@tmp="$$(mktemp -d)"; trap 'rm -rf "$$tmp"' EXIT; \
	$(CC) $(CSTD) $(WARN) harness/probes/smoke.c -o "$$tmp/probe" 2>/dev/null && "$$tmp/probe" \
	  || { $(STATUS) fail "$(CC) cannot compile and run a C11 program"; exit 1; }; \
	$(STATUS) ok "C toolchain: $(CC) $$($(CC) -dumpfullversion -dumpversion) (compiles and runs)"

clean:
	@find . \( -name '*.o' -o -name '*.bin' \) -delete
