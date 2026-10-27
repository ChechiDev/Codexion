#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define TEXT_CAP 8192

static void read_text(const char *path, char *text)
{
	FILE	*file;
	size_t	used;

	file = fopen(path, "r");
	assert(file != NULL);
	used = fread(text, 1, TEXT_CAP - 1, file);
	assert(!ferror(file));
	assert(used < TEXT_CAP - 1 || fgetc(file) == EOF);
	text[used] = '\0';
	assert(fclose(file) == 0);
}

static void run_command(const char *command, char *output)
{
	FILE	*pipe;
	size_t	used;
	int		status;

	pipe = popen(command, "r");
	assert(pipe != NULL);
	used = fread(output, 1, TEXT_CAP - 1, pipe);
	assert(!ferror(pipe));
	assert(used < TEXT_CAP - 1 || fgetc(pipe) == EOF);
	output[used] = '\0';
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC1: is_digit accepts each decimal digit. */
static void test_ac1_all_decimal_digits(void)
{
	char	c;

	c = '0';
	while (c <= '9')
	{
		assert(is_digit(c) == 1);
		c++;
	}
}

/* AC2: is_digit rejects the specified non-digits and negative char. */
static void test_ac2_specified_non_digits(void)
{
	assert(is_digit('/') == 0);
	assert(is_digit(':') == 0);
	assert(is_digit('\0') == 0);
	assert(is_digit(' ') == 0);
	assert(is_digit('+') == 0);
	assert(is_digit('-') == 0);
	assert(is_digit('a') == 0);
	assert(is_digit((char)-1) == 0);
}

/* AC3: the public declaration is in the Utils section of main.h. */
static void test_ac3_utils_prototype_placement(void)
{
	char			text[TEXT_CAP];
	const char		*utils;
	const char		*digit;
	const char		*space;
	const char		*trim;
	const char		*parsing;

	read_text("coders/main.h", text);
	utils = strstr(text, "/* ---- Utils ---- */");
	digit = strstr(text, "is_digit(");
	space = strstr(text, "is_space(");
	trim = strstr(text, "trim_slice(");
	parsing = strstr(text, "/* ---- Parsing ---- */");
	assert(utils != NULL && digit != NULL && space != NULL);
	assert(trim != NULL && parsing != NULL);
	assert(utils < digit && digit < space && space < trim);
	assert(trim < parsing);
}

/* AC4: the deliverable builds with is_digit, does not relink, and passes Norm. */
static void test_ac4_build_no_relink_and_norm(void)
{
	char		text[TEXT_CAP];
	struct stat	info;

	read_text("coders/Makefile", text);
	assert(strstr(text, "src/utils/is_digit.c") != NULL);
	run_command("make -C coders 2>&1", text);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	assert((info.st_mode & 0111) != 0);
	run_command("make -C coders 2>&1", text);
	assert(strstr(text, "Nothing to be done") != NULL);
	run_command("norminette coders/ 2>&1", text);
	assert(strstr(text, "Error") == NULL);
}

int main(void)
{
	test_ac1_all_decimal_digits();
	test_ac2_specified_non_digits();
	test_ac3_utils_prototype_placement();
	test_ac4_build_no_relink_and_norm();
	return (0);
}
