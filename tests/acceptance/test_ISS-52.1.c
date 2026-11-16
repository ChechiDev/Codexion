#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUTPUT_CAP 131072
#define MAX_OBJECTS 128
#define PATH_CAP 256

static char objects[MAX_OBJECTS][PATH_CAP];
static size_t object_count;

static void capture(const char *command, char *output, size_t capacity)
{
	FILE *pipe;
	size_t used;
	size_t count;
	int status;
	char discard[4096];

	assert(capacity > 1);
	pipe = popen(command, "r");
	assert(pipe != NULL);
	used = 0;
	while (used < capacity - 1 && (count = fread(output + used, 1,
		capacity - 1 - used, pipe)) != 0)
		used += count;
	output[used] = '\0';
	while (fread(discard, 1, sizeof(discard), pipe) != 0)
		assert(0 && "make output exceeded capture capacity");
	assert(!ferror(pipe) && feof(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

static char *run(const char *command)
{
	char *output;

	output = malloc(OUTPUT_CAP);
	assert(output != NULL);
	capture(command, output, OUTPUT_CAP);
	return (output);
}

static void load_objects(void)
{
	char *output;
	char *line;
	size_t length;

	output = run("make -s -C coders --eval='show-objects:;@printf \"%s\\n\" $(OBJ)' show-objects");
	line = strtok(output, "\n");
	while (line != NULL)
	{
		length = strlen(line);
		assert(object_count < MAX_OBJECTS && length < PATH_CAP);
		assert(length > 2 && strcmp(line + length - 2, ".o") == 0);
		memcpy(objects[object_count], line, length + 1);
		object_count++;
		line = strtok(NULL, "\n");
	}
	assert(object_count > 1);
	free(output);
}

static int file_exists(const char *path)
{
	struct stat info;

	return (stat(path, &info) == 0);
}

static void assert_objects_exist(int expected)
{
	char path[PATH_CAP + 8];
	size_t i;

	i = 0;
	while (i < object_count)
	{
		assert(snprintf(path, sizeof(path), "coders/%s", objects[i]) > 0);
		assert(file_exists(path) == expected);
		i++;
	}
}

static void assert_no_objects(void)
{
	char *output;

	output = run("find coders -type f -name '*.o'");
	assert(output[0] == '\0');
	free(output);
}

static size_t lines(char *output, char **result, size_t capacity)
{
	char *line;
	size_t count;

	count = 0;
	line = strtok(output, "\n");
	while (line != NULL)
	{
		assert(count < capacity);
		result[count++] = line;
		line = strtok(NULL, "\n");
	}
	return (count);
}

static size_t find_line(char **text, size_t count, const char *wanted)
{
	size_t i;

	i = 0;
	while (i < count && strcmp(text[i], wanted) != 0)
		i++;
	assert(i < count);
	return (i);
}

static void assert_link_block(char **text, size_t count)
{
	char expected[PATH_CAP + 8];
	size_t first;
	size_t i;
	size_t paths;
	const char *match;

	first = find_line(text, count, "cc -Wall -Wextra -Werror -pthread -I. \\");
	assert(first + object_count + 1 < count);
	i = 0;
	while (i < object_count)
	{
		assert(snprintf(expected, sizeof(expected), "    %s \\", objects[i]) > 0);
		assert(strcmp(text[first + i + 1], expected) == 0);
		i++;
	}
	assert(strcmp(text[first + object_count + 1], "    -o codexion") == 0);
	i = 0;
	while (i < count)
	{
		paths = 0;
		match = text[i];
		while ((match = strstr(match, ".o")) != NULL)
		{
			paths++;
			match += 2;
		}
		assert(paths <= 1);
		i++;
	}
}

static void assert_compile_lines(char **text, size_t count)
{
	char expected[PATH_CAP * 2 + 80];
	char source[PATH_CAP];
	size_t first;
	size_t i;
	size_t length;

	length = strlen(objects[0]);
	assert(length > 2);
	memcpy(source, objects[0], length - 2);
	memcpy(source + length - 2, ".c", 3);
	assert(snprintf(expected, sizeof(expected),
		"cc -Wall -Wextra -Werror -pthread -I. -c %s -o %s",
		source, objects[0]) > 0);
	first = find_line(text, count, expected);
	assert(first + object_count <= count);
	i = 0;
	while (i < object_count)
	{
		length = strlen(objects[i]);
		memcpy(source, objects[i], length - 2);
		memcpy(source + length - 2, ".c", 3);
		assert(snprintf(expected, sizeof(expected),
			"cc -Wall -Wextra -Werror -pthread -I. -c %s -o %s",
			source, objects[i]) > 0);
		assert(strcmp(text[first + i], expected) == 0);
		i++;
	}
}

static size_t assert_clean_lines(char **text, size_t count, int with_binary)
{
	char expected[PATH_CAP + 8];
	size_t first;
	size_t i;

	assert(snprintf(expected, sizeof(expected), "rm -f %s", objects[0]) > 0);
	first = find_line(text, count, expected);
	assert(first + object_count <= count);
	i = 0;
	while (i < object_count)
	{
		assert(snprintf(expected, sizeof(expected), "rm -f %s", objects[i]) > 0);
		assert(strcmp(text[first + i], expected) == 0);
		i++;
	}
	i = 0;
	while (i < count)
	{
		if (strncmp(text[i], "rm ", 3) == 0)
			assert((i >= first && i < first + object_count)
				|| (with_binary && i == first + object_count
					&& strcmp(text[i], "rm -f codexion") == 0));
		i++;
	}
	return (first + object_count);
}

/* AC1: link prints the flags, one object per line in OBJ order, and output. */
static void test_ac1_link_output(void)
{
	char *output;
	char *text[1024];
	struct stat info;
	size_t count;

	output = run("make -C coders fclean");
	free(output);
	output = run("make -C coders");
	count = lines(output, text, 1024);
	assert_link_block(text, count);
	assert(stat("coders/codexion", &info) == 0);
	assert(S_ISREG(info.st_mode) && (info.st_mode & 0111) != 0);
	free(output);
}

/* AC2: each object keeps its own compile command in the build output. */
static void test_ac2_compile_output_unchanged(void)
{
	char *output;
	char *text[1024];
	size_t count;

	output = run("make -C coders fclean");
	free(output);
	output = run("make -C coders");
	count = lines(output, text, 1024);
	assert_compile_lines(text, count);
	free(output);
}

/* AC3: clean prints one removal per object and preserves the executable. */
static void test_ac3_clean_output(void)
{
	char *output;
	char *text[1024];
	size_t count;

	assert_objects_exist(1);
	output = run("make -C coders clean");
	count = lines(output, text, 1024);
	assert_clean_lines(text, count, 0);
	assert_objects_exist(0);
	assert_no_objects();
	assert(file_exists("coders/codexion"));
	free(output);
}

/* AC4: fclean removes objects and binary; re removes and rebuilds both. */
static void test_ac4_fclean_and_re(void)
{
	char *output;
	char *text[1024];
	size_t count;
	size_t after_clean;

	output = run("make -C coders");
	free(output);
	output = run("make -C coders fclean");
	count = lines(output, text, 1024);
	after_clean = assert_clean_lines(text, count, 1);
	assert(after_clean < count && strcmp(text[after_clean], "rm -f codexion") == 0);
	assert_objects_exist(0);
	assert_no_objects();
	assert(!file_exists("coders/codexion"));
	free(output);
	output = run("make -C coders re");
	count = lines(output, text, 1024);
	after_clean = assert_clean_lines(text, count, 1);
	assert(after_clean < count && strcmp(text[after_clean], "rm -f codexion") == 0);
	assert_compile_lines(text, count);
	assert_link_block(text, count);
	assert_objects_exist(1);
	assert(file_exists("coders/codexion"));
	free(output);
}

/* AC5: an up-to-date build does not relink; dry run retains the real link. */
static void test_ac5_no_relink(void)
{
	char *output;
	char *text[1024];
	struct stat before;
	struct stat after;
	size_t count;
	size_t i;
	size_t link_count;

	assert(stat("coders/codexion", &before) == 0);
	output = run("make -C coders");
	assert(strstr(output, "cc ") == NULL);
	assert(stat("coders/codexion", &after) == 0);
	assert(before.st_mtim.tv_sec == after.st_mtim.tv_sec);
	assert(before.st_mtim.tv_nsec == after.st_mtim.tv_nsec);
	free(output);
	output = run("make -C coders fclean");
	free(output);
	output = run("make -C coders -n");
	count = lines(output, text, 1024);
	link_count = 0;
	i = 0;
	while (i < count)
	{
		if (strncmp(text[i], "cc ", 3) == 0
			&& strstr(text[i], " -o codexion") != NULL)
		{
			assert(strstr(text[i], "-Wall -Wextra -Werror -pthread") != NULL);
			link_count++;
		}
		i++;
	}
	assert(link_count == 1);
	free(output);
	output = run("make -C coders");
	free(output);
}

int main(void)
{
	load_objects();
	test_ac1_link_output();
	test_ac2_compile_output_unchanged();
	test_ac3_clean_output();
	test_ac4_fclean_and_re();
	test_ac5_no_relink();
	return (0);
}
