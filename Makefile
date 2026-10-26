# Makefile — dev tooling, NOT delivered. The deliverable is coders/Makefile.
# Keep the harness contract targets: lint / type / test / verify / check / plan
# (+ env-check, used by harness/check.sh). CI uses verify / memcheck / tsan
# (harness/ is a local symlink, so check / plan / status only run locally).
.PHONY: lint type test verify check plan status env-check clean build memcheck tsan

CC      := cc
CSTD    := -std=c11
WARN    := -Wall -Wextra -Werror
CFLAGS  := $(WARN) -pthread -Icoders $(EXTRA_CFLAGS)
TSAN    := -fsanitize=thread -g
# TSan aborts under the high ASLR entropy of recent kernels: run without ASLR.
TSAN_RUN := setarch $(shell uname -m) -R
# Colored status tags via the harness; plain printf when harness/ is absent (CI).
STATUS  := $(if $(shell test -x harness/status.sh && echo y),./harness/status.sh,sh -c 'printf "[%s] %s\n" "$$0" "$$*"')

DELIV   := coders
BIN     := $(DELIV)/codexion
BUILD   := build

# Objects for tests: every deliverable source except main.c (tests bring their own main).
SRC      := $(shell find $(DELIV) -name '*.c' ! -path '$(DELIV)/main.c' 2>/dev/null)
HDR      := $(shell find $(DELIV) -name '*.h' 2>/dev/null)
OBJ      := $(SRC:%.c=$(BUILD)/obj/%.o)
TSAN_OBJ := $(SRC:%.c=$(BUILD)/tsan/%.o)
TESTS    := $(shell find tests -name 'test_*.c' 2>/dev/null)
UNIT     := $(filter tests/unit/%,$(TESTS))

# Runs exercised by memcheck / tsan; '|' separates runs. Grows with the project.
SCENARIOS := 4 410 200 200 200 3 0 edf|5 800 200 200 200 2 10 fifo|1 800 200 200 200 1 0 edf|2 300 100 100 100 2 20 fifo|4 410 200 x 200 3 0 edf
VALGRIND  := valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=42 -q

$(BUILD)/obj/%.o: %.c $(HDR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/tsan/%.o: %.c $(HDR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TSAN) -c $< -o $@

# The deliverable Makefile is the only one that builds codexion.
build:
	@if [ -f $(DELIV)/Makefile ]; then $(MAKE) --no-print-directory -C $(DELIV); \
	else $(STATUS) skip "no $(DELIV)/Makefile yet"; fi

# Norm is the style gate (clang-format would fight it). cppcheck is optional.
lint:
	@if [ -z "$$(find $(DELIV) -name '*.[ch]' 2>/dev/null)" ]; then \
	  $(STATUS) skip "no C sources in $(DELIV)/ yet"; \
	elif command -v norminette >/dev/null 2>&1; then \
	  norminette $(DELIV) >/dev/null || { norminette $(DELIV) | grep -v ': OK!'; $(STATUS) fail "norminette"; exit 1; }; \
	  $(STATUS) ok "norminette"; \
	else $(STATUS) skip "norminette not installed"; fi
	@if command -v cppcheck >/dev/null 2>&1 && [ -n "$(SRC)" ]; then \
	  echo "cppcheck"; cppcheck --enable=warning,style --error-exitcode=1 --quiet -I$(DELIV) $(DELIV); \
	else $(STATUS) skip "cppcheck not installed"; fi

# C has no separate type checker: the warning-clean deliverable build IS the gate,
# plus the subject's "no relink" rule (a second make must not run the compiler).
type: build $(OBJ)
	@if [ -f $(DELIV)/Makefile ]; then \
	  if $(MAKE) -s --no-print-directory -C $(DELIV) -n | grep -qE '(^|[ /])(cc|gcc|clang) '; then \
	    $(STATUS) fail "$(DELIV)/Makefile relinks"; exit 1; fi; \
	fi
	@$(STATUS) ok "compiles clean ($(WARN) -pthread), no relink"

# Each tests/**/test_*.c has its own main(); simulation tests run $$CODEXION_BIN.
test: type
	@if [ -z "$(TESTS)" ]; then $(STATUS) ok "no tests yet"; else \
	  for t in $(TESTS); do \
	    bin=$(BUILD)/$${t%.c}.bin; mkdir -p $$(dirname $$bin); \
	    $(CC) $(CFLAGS) $$t $(OBJ) -o $$bin || { $(STATUS) fail "$$t (build)"; exit 1; }; \
	    CODEXION_BIN=$(BIN) ./$$bin || { $(STATUS) fail "$$t"; exit 1; }; \
	  done; \
	  $(STATUS) ok "tests passed"; \
	fi

verify: lint type test

# Leaks / memory errors: unit tests and the scenario runs under valgrind.
memcheck: type
	@command -v valgrind >/dev/null 2>&1 || { $(STATUS) fail "valgrind not installed"; exit 1; }
	@for t in $(UNIT); do \
	  bin=$(BUILD)/$${t%.c}.bin; mkdir -p $$(dirname $$bin); \
	  $(CC) $(CFLAGS) -g $$t $(OBJ) -o $$bin || exit 1; \
	  $(VALGRIND) ./$$bin >/dev/null || { $(STATUS) fail "valgrind: $$t"; exit 1; }; \
	done
	@if [ -x $(BIN) ]; then \
	  echo '$(SCENARIOS)' | tr '|' '\n' | while read -r args; do \
	    $(VALGRIND) ./$(BIN) $$args >/dev/null; \
	    [ $$? -ne 42 ] || { $(STATUS) fail "valgrind: codexion $$args"; exit 1; }; \
	  done || exit 1; \
	else $(STATUS) skip "no $(BIN) yet: scenario runs skipped"; fi
	@$(STATUS) ok "valgrind clean"

# Data races: unit tests and the scenario runs on a -fsanitize=thread build.
tsan: $(TSAN_OBJ)
	@for t in $(UNIT); do \
	  bin=$(BUILD)/tsan/$${t%.c}.bin; mkdir -p $$(dirname $$bin); \
	  $(CC) $(CFLAGS) $(TSAN) $$t $(TSAN_OBJ) -o $$bin || exit 1; \
	  TSAN_OPTIONS=exitcode=66 $(TSAN_RUN) ./$$bin >/dev/null || { $(STATUS) fail "tsan: $$t"; exit 1; }; \
	done
	@if [ -f $(DELIV)/main.c ]; then \
	  mkdir -p $(BUILD)/tsan; \
	  $(CC) $(CFLAGS) $(TSAN) $(DELIV)/main.c $(TSAN_OBJ) -o $(BUILD)/tsan/codexion || exit 1; \
	  echo '$(SCENARIOS)' | tr '|' '\n' | while read -r args; do \
	    TSAN_OPTIONS=exitcode=66 $(TSAN_RUN) ./$(BUILD)/tsan/codexion $$args >/dev/null 2>$(BUILD)/tsan/last.log; \
	    [ $$? -ne 66 ] || { cat $(BUILD)/tsan/last.log; $(STATUS) fail "tsan: codexion $$args"; exit 1; }; \
	  done || exit 1; \
	else $(STATUS) skip "no $(DELIV)/main.c yet: scenario runs skipped"; fi
	@$(STATUS) ok "tsan clean"

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
	@rm -rf $(BUILD)
	@if [ -f $(DELIV)/Makefile ]; then $(MAKE) --no-print-directory -C $(DELIV) fclean; fi
