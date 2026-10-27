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

/* AC1: main.h exposes t_slice with a const start and an int length. */
static void test_ac1_public_slice_type(void)
{
	t_slice			slice;
	const char		*start;
	int				len;

	assert(_Generic(slice.start, const char *: 1, default: 0));
	assert(_Generic(slice.len, int: 1, default: 0));
	slice.start = "x";
	slice.len = 1;
	start = slice.start;
	len = slice.len;
	assert(start[0] == 'x' && len == 1);
}

/* AC2: padded and plain inputs return views into the original strings. */
static void test_ac2_basic_trim(void)
{
	const char	*padded;
	const char	*plain;
	t_slice		slice;

	padded = "     0007     ";
	plain = "fifo";
	slice = trim_slice(padded);
	assert(slice.start == padded + 5 && slice.len == 4);
	slice = trim_slice(plain);
	assert(slice.start == plain && slice.len == 4);
}

/* AC3: all six outer whitespace bytes trim; inner whitespace remains. */
static void test_ac3_whitespace_and_interior(void)
{
	const char	*outer;
	const char	*inner;
	t_slice		slice;

	outer = " \t\n\v\f\r edf \r\f\v\n\t ";
	inner = "  fi fo  ";
	slice = trim_slice(outer);
	assert(slice.start == strchr(outer, 'e') && slice.len == 3);
	assert(strncmp(slice.start, "edf", (size_t)slice.len) == 0);
	slice = trim_slice(inner);
	assert(slice.start == inner + 2 && slice.len == 5);
	assert(strncmp(slice.start, "fi fo", (size_t)slice.len) == 0);
}

/* AC4: empty, all-space and NULL inputs are safe; bytes are unchanged. */
static void test_ac4_empty_null_and_unchanged(void)
{
	char		inputs[3][32];
	char		original[3][32];
	t_slice		slice;
	int			i;

	strcpy(inputs[0], "");
	strcpy(inputs[1], "   ");
	strcpy(inputs[2], " \t0007 fi\r\n ");
	memcpy(original, inputs, sizeof(inputs));
	i = 0;
	while (i < 3)
	{
		slice = trim_slice(inputs[i]);
		assert(memcmp(inputs[i], original[i], sizeof(inputs[i])) == 0);
		if (i < 2)
			assert(slice.len == 0);
		else
			assert(slice.start == inputs[i] + 2 && slice.len == 7);
		i++;
	}
	slice = trim_slice(NULL);
	assert(slice.start == NULL && slice.len == 0);
}

/* AC5: the trim_slice prototype is inside the Utils header section. */
static void test_ac5_utils_prototype(void)
{
	char		text[TEXT_CAP];
	const char	*utils;
	const char	*parsing;
	const char	*prototype;

	read_text("coders/main.h", text);
	utils = strstr(text, "/* ---- Utils ---- */");
	parsing = strstr(text, "/* ---- Parsing ---- */");
	prototype = strstr(text, "trim_slice(");
	assert(utils != NULL && parsing != NULL && prototype != NULL);
	assert(utils < prototype && prototype < parsing);
}

/* AC6: the new source builds, does not relink, and passes Norm. */
static void test_ac6_build_no_relink_and_norm(void)
{
	char		text[TEXT_CAP];
	struct stat	info;

	read_text("coders/Makefile", text);
	assert(strstr(text, "src/utils/trim_slice.c") != NULL);
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
	test_ac1_public_slice_type();
	test_ac2_basic_trim();
	test_ac3_whitespace_and_interior();
	test_ac4_empty_null_and_unchanged();
	test_ac5_utils_prototype();
	test_ac6_build_no_relink_and_norm();
	return (0);
}
