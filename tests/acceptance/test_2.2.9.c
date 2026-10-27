#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* AC1: trim_slice preserves all six edge spaces, inner spaces, empty and NULL input, and source bytes. */
static void test_ac1_trim_slice_regression(void)
{
	char		input[] = " \t\n\v\f\r a \t\n\v\f\rb \r\f\v\n\t ";
	char		original[sizeof(input)];
	const char	*spaces = " \t\n\v\f\r";
	char		pair[] = "xAx";
	t_slice		slice;
	int			i;

	memcpy(original, input, sizeof(input));
	slice = trim_slice(input);
	assert(slice.start == input + 7);
	assert(slice.len == 8);
	assert(strncmp(slice.start, "a \t\n\v\f\rb", (size_t)slice.len) == 0);
	assert(memcmp(input, original, sizeof(input)) == 0);
	i = 0;
	while (spaces[i] != '\0')
	{
		pair[0] = spaces[i];
		pair[2] = spaces[i];
		slice = trim_slice(pair);
		assert(slice.start == pair + 1 && slice.len == 1 && *slice.start == 'A');
		i++;
	}
	slice = trim_slice("");
	assert(slice.len == 0);
	slice = trim_slice("   ");
	assert(slice.len == 0);
	slice = trim_slice(NULL);
	assert(slice.start == NULL && slice.len == 0);
}

/* AC2: bytes outside the six whitespace characters remain at both boundaries. */
static void test_ac2_non_whitespace_boundaries(void)
{
	const char	*controls = "\x01" "ab" "\x7f";
	const char	*underscores = "_x_";
	t_slice		slice;

	slice = trim_slice(controls);
	assert(slice.start == controls && slice.len == 4);
	assert((unsigned char)slice.start[0] == 1);
	assert((unsigned char)slice.start[3] == 127);
	slice = trim_slice(underscores);
	assert(slice.start == underscores && slice.len == 3);
}

static void run_command(const char *command)
{
	FILE	*pipe;
	char	output[8192];
	size_t	used;
	int		status;

	pipe = popen(command, "r");
	assert(pipe != NULL);
	used = fread(output, 1, sizeof(output) - 1, pipe);
	assert(!ferror(pipe));
	assert(used < sizeof(output) - 1 || fgetc(pipe) == EOF);
	output[used] = '\0';
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(strstr(output, "Error") == NULL);
}

/* AC3: the deliverable builds and norminette reports no error. */
static void test_ac3_build_and_norm(void)
{
	struct stat	info;

	run_command("make -C coders 2>&1");
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	run_command("norminette coders/ 2>&1");
}

int main(void)
{
	test_ac1_trim_slice_regression();
	test_ac2_non_whitespace_boundaries();
	test_ac3_build_and_norm();
	return (0);
}
