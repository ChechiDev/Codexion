#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

static void assert_source_rejected(const char *source)
{
	FILE *compiler;
	int status;

	compiler = popen("cc -std=c11 -Wall -Wextra -Werror -pedantic "
		"-Icoders -x c -fsyntax-only - 2>/dev/null", "w");
	assert(compiler != NULL);
	assert(fputs(source, compiler) >= 0);
	status = pclose(compiler);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) != 0);
}

/* AC1: t_config exposes seven int fields and a readable t_mode field. */
static void test_ac1_config_mode_field(void)
{
	t_config config;

	assert(_Generic(config.number_of_coders, int: 1, default: 0));
	assert(_Generic(config.time_to_burnout, int: 1, default: 0));
	assert(_Generic(config.time_to_compile, int: 1, default: 0));
	assert(_Generic(config.time_to_debug, int: 1, default: 0));
	assert(_Generic(config.time_to_refactor, int: 1, default: 0));
	assert(_Generic(config.number_of_compiles_required, int: 1, default: 0));
	assert(_Generic(config.dongle_cooldown, int: 1, default: 0));
	assert(_Generic(config.mode, t_mode: 1, default: 0));
	config.number_of_coders = 1;
	config.time_to_burnout = 2;
	config.time_to_compile = 3;
	config.time_to_debug = 4;
	config.time_to_refactor = 5;
	config.number_of_compiles_required = 6;
	config.dongle_cooldown = 7;
	config.mode = MODE_FIFO;
	assert(config.number_of_coders == 1 && config.time_to_burnout == 2);
	assert(config.time_to_compile == 3 && config.time_to_debug == 4);
	assert(config.time_to_refactor == 5);
	assert(config.number_of_compiles_required == 6);
	assert(config.dongle_cooldown == 7 && config.mode == MODE_FIFO);
}

/* AC2: MODE_FIFO and MODE_EDF are distinct t_mode values. */
static void test_ac2_distinct_modes(void)
{
	t_mode fifo;
	t_mode edf;

	fifo = MODE_FIFO;
	edf = MODE_EDF;
	assert(fifo != edf);
}

/* AC3: each former public name is rejected by a separate translation unit. */
static void test_ac3_old_names_absent(void)
{
	assert_source_rejected("#include \"main.h\"\n"
		"int main(void) { return sizeof(t_policy) == 0; }\n");
	assert_source_rejected("#include \"main.h\"\n"
		"int main(void) { return POLICY_FIFO; }\n");
	assert_source_rejected("#include \"main.h\"\n"
		"int main(void) { return POLICY_EDF; }\n");
	assert_source_rejected("#include \"main.h\"\n"
		"int main(void) { return sizeof(((t_config *)0)->scheduler) == 0; }\n");
}

/* AC4: norminette reports no error for the deliverable directory. */
static void test_ac4_norminette_clean(void)
{
	FILE *norm;
	char output[8192];
	size_t used;
	size_t count;
	int status;

	norm = popen("norminette coders/ 2>&1", "r");
	assert(norm != NULL);
	used = 0;
	while (used < sizeof(output) - 1 && (count = fread(output + used, 1,
		sizeof(output) - 1 - used, norm)) != 0)
		used += count;
	output[used] = '\0';
	assert(feof(norm));
	status = pclose(norm);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(strstr(output, "Error") == NULL);
}

int main(void)
{
	test_ac1_config_mode_field();
	test_ac2_distinct_modes();
	test_ac3_old_names_absent();
	test_ac4_norminette_clean();
	return (0);
}
