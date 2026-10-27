#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define OUTPUT_CAP 8192

static void assert_source_rejected(const char *source)
{
	FILE	*compiler;
	int		status;

	compiler = popen("cc -std=c11 -Wall -Wextra -Werror -pedantic "
		"-Icoders -x c -fsyntax-only - 2>/dev/null", "w");
	assert(compiler != NULL);
	assert(fputs(source, compiler) >= 0);
	status = pclose(compiler);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) != 0);
}

/* AC1: the former public type, struct tag and function are unavailable. */
static void test_ac1_old_names_absent(void)
{
	assert_source_rejected("#include \"main.h\"\n"
		"int main(void) { return sizeof(t_span) == 0; }\n");
	assert_source_rejected("#include \"main.h\"\n"
		"int main(void) { return sizeof(struct s_span) == 0; }\n");
	assert_source_rejected("#include \"main.h\"\n"
		"int main(void) { return trim_span(\"x\").len; }\n");
}

/* AC2: the successor API and numeric parsing retain their public behavior. */
static void test_ac2_slice_and_number_regression(void)
{
	const char	*input;
	t_slice		slice;
	int			n;

	input = "  fi fo  ";
	slice = trim_slice(input);
	assert(slice.start == input + 2 && slice.len == 5);
	assert(strncmp(slice.start, "fi fo", (size_t)slice.len) == 0);
	slice = trim_slice(NULL);
	assert(slice.start == NULL && slice.len == 0);
	n = -1;
	assert(parse_number("  42  ", &n) == 0 && n == 42);
}

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

/* AC3: the deliverable builds once, does not relink, and passes Norm. */
static void test_ac3_build_no_relink_and_norm(void)
{
	struct stat	info;
	char		output[OUTPUT_CAP];

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
	test_ac1_old_names_absent();
	test_ac2_slice_and_number_regression();
	test_ac3_build_no_relink_and_norm();
	return (0);
}
