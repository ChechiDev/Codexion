#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define OUTPUT_CAP 8192

static void run_command(const char *command, char *output)
{
	FILE	*pipe;
	size_t	used;
	int		status;

	pipe = popen(command, "r");
	assert(pipe != NULL);
	used = fread(output, 1, OUTPUT_CAP - 1, pipe);
	assert(!ferror(pipe));
	assert(used < OUTPUT_CAP - 1 || fgetc(pipe) == EOF);
	output[used] = '\0';
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC1: parse_number still trims both ends and rejects inner whitespace. */
static void test_ac1_trimmed_numbers(void)
{
	int	n;

	n = -1;
	assert(parse_number("  42  ", &n) == 0 && n == 42);
	n = -1;
	assert(parse_number(" \t\n\v\f\r 7 \r\f\v\n\t ", &n) == 0 && n == 7);
	n = 123;
	assert(parse_number("4 2", &n) == -1 && n == 123);
}

/* AC2: the moved source builds, does not relink, and passes Norm. */
static void test_ac2_build_no_relink_and_norm(void)
{
	FILE		*makefile;
	char	output[OUTPUT_CAP];
	struct stat	info;
	size_t	used;

	makefile = fopen("coders/Makefile", "r");
	assert(makefile != NULL);
	used = fread(output, 1, OUTPUT_CAP - 1, makefile);
	assert(!ferror(makefile));
	assert(used < OUTPUT_CAP - 1 || fgetc(makefile) == EOF);
	output[used] = '\0';
	assert(fclose(makefile) == 0);
	assert(strstr(output, "src/utils/trim_span.c") != NULL);
	assert(strstr(output, "src/parsing/trim_span.c") == NULL);
	run_command("make -C coders 2>&1", output);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	assert((info.st_mode & 0111) != 0);
	run_command("make -C coders 2>&1", output);
	assert(strstr(output, "Nothing to be done") != NULL);
	run_command("norminette coders/ 2>&1", output);
	assert(strstr(output, "Error") == NULL);
}

int main(void)
{
	test_ac1_trimmed_numbers();
	test_ac2_build_no_relink_and_norm();
	return (0);
}
