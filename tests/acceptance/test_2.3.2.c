#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

static void expect_rejection(const char *input)
{
	t_mode	mode;

	mode = MODE_EDF;
	assert(parse_mode(input, &mode) == -1);
	assert(mode == MODE_EDF);
	mode = MODE_FIFO;
	assert(parse_mode(input, &mode) == -1);
	assert(mode == MODE_FIFO);
}

/* AC1: exact fifo and edf strings select their corresponding modes. */
static void test_ac1_exact_modes(void)
{
	t_mode	mode;

	mode = MODE_EDF;
	assert(parse_mode("fifo", &mode) == 0);
	assert(mode == MODE_FIFO);
	mode = MODE_FIFO;
	assert(parse_mode("edf", &mode) == 0);
	assert(mode == MODE_EDF);
}

/* AC2: case, whitespace, partial, extended, and other strings are rejected. */
static void test_ac2_invalid_modes_leave_output_unchanged(void)
{
	static const char	*inputs[] = {
		"FIFO", "Edf", "FiFo", "fifo ", " edf", "edf\n", "\tfifo",
		"fif", "edff", "fifoedf", "", "rr", "0"
	};
	size_t			index;

	index = 0;
	while (index < sizeof(inputs) / sizeof(inputs[0]))
	{
		expect_rejection(inputs[index]);
		index++;
	}
}

/* AC3: either null pointer is rejected safely. */
static void test_ac3_null_pointers(void)
{
	t_mode	mode;

	mode = MODE_EDF;
	assert(parse_mode(NULL, &mode) == -1);
	assert(mode == MODE_EDF);
	assert(parse_mode("fifo", NULL) == -1);
	assert(parse_mode(NULL, NULL) == -1);
}

static void run_command(const char *command)
{
	int	status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC4: the deliverable builds with the parser and passes Norm. */
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
	assert(strstr(text, "src/parsing/parse_mode.c") != NULL);
	run_command("make -C coders >/dev/null 2>&1");
	assert(stat("coders/codexion", &info) == 0);
	assert(S_ISREG(info.st_mode));
	assert((info.st_mode & 0111) != 0);
	run_command("norminette coders/ >/dev/null 2>&1");
}

int	main(void)
{
	test_ac1_exact_modes();
	test_ac2_invalid_modes_leave_output_unchanged();
	test_ac3_null_pointers();
	test_ac4_build_and_norm();
	return (0);
}
