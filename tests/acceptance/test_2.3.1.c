#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

static void run_command(const char *command)
{
	int	status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC1: positions 1 and 2 require a minimum value of 1. */
static void test_ac1_positive_minimums(void)
{
	assert(get_arg_minimum(1) == 1);
	assert(get_arg_minimum(2) == 1);
}

/* AC2: numeric positions 3 through 7 accept zero. */
static void test_ac2_zero_minimums(void)
{
	int	position;

	position = 3;
	while (position <= 7)
	{
		assert(get_arg_minimum(position) == 0);
		position++;
	}
}

/* AC3: the scheduler and out-of-range positions have no numeric minimum. */
static void test_ac3_non_numeric_positions(void)
{
	assert(get_arg_minimum(8) == -1);
	assert(get_arg_minimum(0) == -1);
	assert(get_arg_minimum(9) == -1);
	assert(get_arg_minimum(-1) == -1);
}

/* AC4: the new source builds into codexion and the deliverable passes Norm. */
static void test_ac4_build_and_norm(void)
{
	char		text[4096];
	FILE		*file;
	size_t		used;
	struct stat	info;

	file = fopen("coders/Makefile", "r");
	assert(file != NULL);
	used = fread(text, 1, sizeof(text) - 1, file);
	assert(!ferror(file));
	assert(fclose(file) == 0);
	text[used] = '\0';
	assert(strstr(text, "src/parsing/get_arg_minimum.c") != NULL);
	run_command("make -C coders >/dev/null 2>&1");
	assert(stat("coders/codexion", &info) == 0);
	assert(S_ISREG(info.st_mode));
	assert((info.st_mode & 0111) != 0);
	run_command("norminette coders/ >/dev/null 2>&1");
}

int	main(void)
{
	test_ac1_positive_minimums();
	test_ac2_zero_minimums();
	test_ac3_non_numeric_positions();
	test_ac4_build_and_norm();
	return (0);
}
