#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

static void capture(const char *command, char *output, size_t capacity)
{
	FILE *pipe;
	size_t used;
	size_t count;
	int status;
	char discard[4096];

	assert(capacity > 0);
	pipe = popen(command, "r");
	assert(pipe != NULL);
	used = 0;
	while (used < capacity - 1 && (count = fread(output + used, 1,
		capacity - 1 - used, pipe)) != 0)
		used += count;
	output[used] = '\0';
	while (fread(discard, 1, sizeof(discard), pipe) != 0)
		;
	assert(!ferror(pipe));
	assert(feof(pipe));
	status = pclose(pipe);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

static int exists(const char *path)
{
	struct stat info;

	return (stat(path, &info) == 0);
}

static long object_count(void)
{
	char output[128];
	char *end;
	long count;

	capture("find coders -type f -name '*.o' | wc -l", output,
		sizeof(output));
	count = strtol(output, &end, 10);
	assert(end != output && count >= 0);
	return (count);
}

static char *read_file(const char *path)
{
	FILE *file;
	char *contents;
	long size;

	file = fopen(path, "rb");
	assert(file != NULL);
	assert(fseek(file, 0, SEEK_END) == 0);
	size = ftell(file);
	assert(size >= 0);
	assert(fseek(file, 0, SEEK_SET) == 0);
	contents = malloc((size_t)size + 1);
	assert(contents != NULL);
	assert(fread(contents, 1, (size_t)size, file) == (size_t)size);
	contents[size] = '\0';
	assert(fclose(file) == 0);
	return (contents);
}

static int has_emoji(const unsigned char *text)
{
	unsigned int codepoint;

	while (*text != 0)
	{
		if (text[0] == 0xf0 && text[1] != 0 && text[2] != 0
			&& text[3] != 0 && (text[1] & 0xc0) == 0x80
			&& (text[2] & 0xc0) == 0x80 && (text[3] & 0xc0) == 0x80)
		{
			codepoint = ((unsigned int)(text[0] & 7) << 18)
				| ((unsigned int)(text[1] & 63) << 12)
				| ((unsigned int)(text[2] & 63) << 6) | (text[3] & 63);
			if (codepoint >= 0x1f000 && codepoint <= 0x1faff)
				return (1);
			text += 4;
		}
		else if (text[0] == 0xe2 && text[1] != 0 && text[2] != 0
			&& text[1] == 0x9d
			&& (unsigned char)text[2] >= 0xa4
			&& (unsigned char)text[2] <= 0xbf)
			return (1);
		else
			text++;
	}
	return (0);
}

static int contains_word(const char *text, const char *word)
{
	const char *match;
	size_t length;

	length = strlen(word);
	match = strstr(text, word);
	while (match != NULL)
	{
		if ((match == text || (!isalnum((unsigned char)match[-1])
				&& match[-1] != '_'))
			&& !isalnum((unsigned char)match[length])
			&& match[length] != '_')
			return (1);
		match = strstr(match + 1, word);
	}
	return (0);
}

/* AC1: make -C coders creates executable coders/codexion and exits 0. */
static void test_ac1_builds_executable(void)
{
	char output[8192];
	struct stat info;

	capture("make -C coders", output, sizeof(output));
	assert(stat("coders/codexion", &info) == 0);
	assert(S_ISREG(info.st_mode) && (info.st_mode & 0111) != 0);
}

/* AC2: dry-run compile and link use cc and the required warning/thread flags. */
static void test_ac2_compile_and_link_flags(void)
{
	char output[65536];
	char *line;
	int compile;
	int link;

	capture("make -C coders fclean", output, sizeof(output));
	capture("make -C coders -n", output, sizeof(output));
	compile = 0;
	link = 0;
	line = strtok(output, "\n");
	while (line != NULL)
	{
		if (strncmp(line, "cc ", 3) == 0)
		{
			assert(strstr(line, "-Wall") != NULL);
			assert(strstr(line, "-Wextra") != NULL);
			assert(strstr(line, "-Werror") != NULL);
			assert(strstr(line, "-pthread") != NULL);
			if (strstr(line, " -c ") != NULL)
				compile++;
			else if (strstr(line, " -o ") != NULL)
				link++;
		}
		line = strtok(NULL, "\n");
	}
	assert(compile > 0 && link > 0);
	capture("make -C coders", output, sizeof(output));
}

/* AC3: a second build does not invoke cc or change the binary mtime. */
static void test_ac3_no_relink(void)
{
	char output[8192];
	struct stat before;
	struct stat after;

	capture("make -C coders", output, sizeof(output));
	assert(stat("coders/codexion", &before) == 0);
	capture("make -C coders", output, sizeof(output));
	assert(strstr(output, "cc ") == NULL);
	assert(stat("coders/codexion", &after) == 0);
	assert(before.st_mtim.tv_sec == after.st_mtim.tv_sec);
	assert(before.st_mtim.tv_nsec == after.st_mtim.tv_nsec);
}

/* AC4: clean removes every object and preserves the executable. */
static void test_ac4_clean_preserves_binary(void)
{
	char output[8192];

	capture("make -C coders", output, sizeof(output));
	assert(object_count() > 0);
	capture("make -C coders clean", output, sizeof(output));
	assert(object_count() == 0);
	assert(exists("coders/codexion"));
}

/* AC5: fclean removes every object and the executable. */
static void test_ac5_fclean_removes_binary(void)
{
	char output[8192];

	capture("make -C coders", output, sizeof(output));
	assert(object_count() > 0);
	capture("make -C coders fclean", output, sizeof(output));
	assert(object_count() == 0);
	assert(!exists("coders/codexion"));
}

/* AC6: re recompiles objects and rebuilds the executable from scratch. */
static void test_ac6_rebuilds_from_scratch(void)
{
	char output[8192];

	capture("make -C coders", output, sizeof(output));
	capture("make -C coders re", output, sizeof(output));
	assert(strstr(output, "cc ") != NULL);
	assert(strstr(output, " -c ") != NULL);
	assert(object_count() > 0);
	assert(exists("coders/codexion"));
}

/* AC7: the codexion target builds the executable. */
static void test_ac7_named_target_builds(void)
{
	char output[8192];

	capture("make -C coders fclean", output, sizeof(output));
	capture("make -C coders codexion", output, sizeof(output));
	assert(exists("coders/codexion"));
}

/* AC8: norminette coders/ reports no error. */
static void test_ac8_norminette_clean(void)
{
	char output[8192];

	capture("norminette coders/ 2>&1", output, sizeof(output));
	assert(strstr(output, "Error") == NULL);
}

/* AC9: the README has its required opening, sections, style, and build steps. */
static void test_ac9_readme_build_instructions(void)
{
	char *readme;
	char *instructions;
	char *next_section;
	const char *first_line = "*This project has been created as part of the "
		"42 curriculum by sperez-l.*\n";

	readme = read_file("README.md");
	assert(strncmp(readme, first_line, strlen(first_line)) == 0);
	assert(strstr(readme, "## Description") != NULL);
	instructions = strstr(readme, "## Instructions");
	assert(instructions != NULL);
	assert(strstr(readme, "## Resources") != NULL);
	assert(strstr(readme, "## Blocking cases handled") != NULL);
	assert(strstr(readme, "## Thread synchronization mechanisms") != NULL);
	assert(!has_emoji((const unsigned char *)readme));
	next_section = strstr(instructions + 3, "\n## ");
	if (next_section != NULL)
		*next_section = '\0';
	assert(strstr(instructions, "make -C coders") != NULL);
	assert(contains_word(instructions, "clean"));
	assert(contains_word(instructions, "fclean"));
	assert(contains_word(instructions, "re"));
	free(readme);
}

int main(void)
{
	char output[8192];

	test_ac1_builds_executable();
	test_ac2_compile_and_link_flags();
	test_ac3_no_relink();
	test_ac4_clean_preserves_binary();
	test_ac5_fclean_removes_binary();
	test_ac6_rebuilds_from_scratch();
	test_ac7_named_target_builds();
	test_ac8_norminette_clean();
	test_ac9_readme_build_instructions();
	capture("make -C coders", output, sizeof(output));
	return (0);
}
