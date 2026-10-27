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

/* AC1: is_space recognizes exactly the six specified whitespace bytes. */
static void test_ac1_six_whitespace_characters(void)
{
	const char	*spaces;
	int			i;

	spaces = " \t\n\v\f\r";
	i = 0;
	while (spaces[i] != '\0')
	{
		assert(is_space(spaces[i]) == 1);
		i++;
	}
}

/* AC2: is_space rejects non-whitespace, including a negative char value. */
static void test_ac2_non_whitespace_characters(void)
{
	assert(is_space('\0') == 0);
	assert(is_space('a') == 0);
	assert(is_space('0') == 0);
	assert(is_space('_') == 0);
	assert(is_space((char)127) == 0);
	assert(is_space((char)-1) == 0);
}

/* AC3: the public declaration precedes trim_slice within the Utils section. */
static void test_ac3_utils_prototype_order(void)
{
	char			text[TEXT_CAP];
	const char		*utils;
	const char		*prototype;
	const char		*trim;
	const char		*parsing;

	read_text("coders/main.h", text);
	utils = strstr(text, "/* ---- Utils ---- */");
	prototype = strstr(text, "is_space(");
	trim = strstr(text, "trim_slice(");
	parsing = strstr(text, "/* ---- Parsing ---- */");
	assert(utils != NULL && prototype != NULL && trim != NULL);
	assert(parsing != NULL);
	assert(utils < prototype && prototype < trim && trim < parsing);
}

/* AC4: the deliverable includes the source, builds, does not relink, and passes Norm. */
static void test_ac4_build_no_relink_and_norm(void)
{
	char		text[TEXT_CAP];
	struct stat	info;

	read_text("coders/Makefile", text);
	assert(strstr(text, "src/utils/is_space.c") != NULL);
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
	test_ac1_six_whitespace_characters();
	test_ac2_non_whitespace_characters();
	test_ac3_utils_prototype_order();
	test_ac4_build_no_relink_and_norm();
	return (0);
}
