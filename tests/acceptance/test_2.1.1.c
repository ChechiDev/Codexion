#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* AC1: trim outer spaces and retain a pointer into the original string. */
static void test_ac1_basic_span(void)
{
	const char *padded = "     0007     ";
	const char *plain = "fifo";
	t_span span;

	span = trim_span(padded);
	assert(span.start == padded + 5 && span.len == 4);
	span = trim_span(plain);
	assert(span.start == plain && span.len == 4);
}

/* AC2: all six C-locale whitespace bytes are trimmed on both sides. */
static void test_ac2_six_whitespace_bytes(void)
{
	const char *input = " \t\n\v\f\r edf \r\f\v\n\t ";
	t_span span;

	span = trim_span(input);
	assert(span.start == strchr(input, 'e'));
	assert(span.len == 3);
	assert(strncmp(span.start, "edf", (size_t)span.len) == 0);
}

/* AC3: whitespace between non-whitespace bytes remains in the span. */
static void test_ac3_inner_whitespace_preserved(void)
{
	const char *input = "  fi fo  ";
	t_span span;

	span = trim_span(input);
	assert(span.start == input + 2 && span.len == 5);
	assert(strncmp(span.start, "fi fo", (size_t)span.len) == 0);
}

/* AC4: empty and all-space input have zero length; NULL has a NULL start. */
static void test_ac4_empty_and_null(void)
{
	t_span span;

	span = trim_span("");
	assert(span.len == 0);
	span = trim_span("   ");
	assert(span.len == 0);
	span = trim_span(NULL);
	assert(span.start == NULL && span.len == 0);
}

/* AC5: trimming does not alter any byte of its input. */
static void test_ac5_input_unchanged(void)
{
	char input[] = " \t0007 fi\r\n ";
	char original[sizeof(input)];
	t_span span;

	memcpy(original, input, sizeof(input));
	span = trim_span(input);
	assert(span.start == input + 2 && span.len == 7);
	assert(memcmp(input, original, sizeof(input)) == 0);
}

/* AC6: the deliverable builds with the new source and passes Norm. */
static void test_ac6_build_and_norm(void)
{
	FILE *file;
	FILE *pipe;
	char makefile[8192];
	char output[8192];
	struct stat info;
	size_t used;
	size_t count;
	int status;

	file = fopen("coders/Makefile", "r");
	assert(file != NULL);
	used = fread(makefile, 1, sizeof(makefile) - 1, file);
	assert(!ferror(file));
	assert(used < sizeof(makefile) - 1 || fgetc(file) == EOF);
	makefile[used] = '\0';
	assert(fclose(file) == 0);
	assert(strstr(makefile, "src/utils/trim_span.c") != NULL);
	pipe = popen("make -C coders 2>&1", "r");
	assert(pipe != NULL);
	while (fread(output, 1, sizeof(output), pipe) != 0)
		;
	assert(!ferror(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(stat("coders/codexion", &info) == 0 && S_ISREG(info.st_mode));
	assert((info.st_mode & 0111) != 0);
	pipe = popen("norminette coders/ 2>&1", "r");
	assert(pipe != NULL);
	used = 0;
	while (used < sizeof(output) - 1 && (count = fread(output + used, 1,
		sizeof(output) - 1 - used, pipe)) != 0)
		used += count;
	output[used] = '\0';
	assert(feof(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(strstr(output, "Error") == NULL);
}

int main(void)
{
	test_ac1_basic_span();
	test_ac2_six_whitespace_bytes();
	test_ac3_inner_whitespace_preserved();
	test_ac4_empty_and_null();
	test_ac5_input_unchanged();
	test_ac6_build_and_norm();
	return (0);
}
