#define _POSIX_C_SOURCE 200809L

#include "main.h"
#include "main.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

static void capture(const char *command, char *output, size_t capacity)
{
	FILE *pipe;
	size_t used;
	size_t count;
	int status;

	assert(capacity > 0);
	pipe = popen(command, "r");
	assert(pipe != NULL);
	used = 0;
	while (used < capacity - 1 && (count = fread(output + used, 1,
		capacity - 1 - used, pipe)) != 0)
		used += count;
	output[used] = '\0';
	assert(feof(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC1: t_config exposes the eight fields with their specified types. */
static void test_ac1_config_fields(void)
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
	config.number_of_coders = 2;
	config.time_to_burnout = 3;
	config.time_to_compile = 4;
	config.time_to_debug = 5;
	config.time_to_refactor = 6;
	config.number_of_compiles_required = 7;
	config.dongle_cooldown = 8;
	config.mode = MODE_FIFO;
	assert(config.number_of_coders == 2 && config.time_to_burnout == 3);
	assert(config.time_to_compile == 4 && config.time_to_debug == 5);
	assert(config.time_to_refactor == 6);
	assert(config.number_of_compiles_required == 7);
	assert(config.dongle_cooldown == 8 && config.mode == MODE_FIFO);
}

/* AC2: fifo and edf are distinct t_mode values. */
static void test_ac2_distinct_policies(void)
{
	t_mode fifo;
	t_mode edf;

	fifo = MODE_FIFO;
	edf = MODE_EDF;
	assert(fifo != edf);
}

/* AC3: including main.h twice compiles under the required warning flags. */
static void test_ac3_header_twice_compiles(void)
{
	t_config config;

	config.mode = MODE_EDF;
	assert(config.mode == MODE_EDF);
}

/* AC4: changing main.h triggers object recompilation on the next make. */
static void test_ac4_header_recompiles_objects(void)
{
	char output[8192];
	struct stat header;
	struct stat object;
	struct timespec changed[2];
	struct timespec original[2];

	capture("make -C coders 2>&1", output, sizeof(output));
	assert(stat("coders/main.h", &header) == 0);
	assert(stat("coders/main.o", &object) == 0);
	original[0] = header.st_atim;
	original[1] = header.st_mtim;
	changed[0] = header.st_atim;
	changed[1] = object.st_mtim;
	changed[1].tv_sec += 2;
	assert(utimensat(AT_FDCWD, "coders/main.h", changed, 0) == 0);
	capture("make -C coders 2>&1", output, sizeof(output));
	assert(utimensat(AT_FDCWD, "coders/main.h", original, 0) == 0);
	assert(strstr(output, " -c ") != NULL);
}

/* AC5: norminette reports no error for the deliverable directory. */
static void test_ac5_norminette_clean(void)
{
	char output[8192];

	capture("norminette coders/ 2>&1", output, sizeof(output));
	assert(strstr(output, "Error") == NULL);
}

int main(void)
{
	test_ac1_config_fields();
	test_ac2_distinct_policies();
	test_ac3_header_twice_compiles();
	test_ac4_header_recompiles_objects();
	test_ac5_norminette_clean();
	return (0);
}
