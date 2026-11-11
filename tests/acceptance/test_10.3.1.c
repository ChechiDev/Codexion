#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define README_CAP 131072
#define OUTPUT_CAP 4096

static char g_readme[README_CAP];

static void read_readme(void)
{
	FILE *file;
	size_t used;

	file = fopen("README.md", "rb");
	assert(file != NULL);
	used = fread(g_readme, 1, sizeof(g_readme) - 1, file);
	assert(!ferror(file));
	assert(used < sizeof(g_readme) - 1 || fgetc(file) == EOF);
	assert(memchr(g_readme, '\0', used) == NULL);
	g_readme[used] = '\0';
	assert(fclose(file) == 0);
}

static int heading_count(const char *name)
{
	const char *line;
	const char *end;
	size_t length;
	int count;

	length = strlen(name);
	line = g_readme;
	count = 0;
	while (*line != '\0')
	{
		end = strchr(line, '\n');
		if (end == NULL)
			end = line + strlen(line);
		if ((size_t)(end - line) == length + 3
			&& strncmp(line, "## ", 3) == 0
			&& strncmp(line + 3, name, length) == 0)
			count++;
		line = *end == '\n' ? end + 1 : end;
	}
	return (count);
}

static void section(const char *name, char *out, size_t capacity)
{
	char heading[128];
	const char *start;
	const char *end;
	const char *next;
	size_t length;

	assert(snprintf(heading, sizeof(heading), "## %s\n", name) > 0);
	start = strstr(g_readme, heading);
	assert(start != NULL);
	start += strlen(heading);
	end = g_readme + strlen(g_readme);
	next = strstr(start, "\n## ");
	if (next != NULL && next < end)
		end = next;
	length = (size_t)(end - start);
	assert(length < capacity);
	memcpy(out, start, length);
	out[length] = '\0';
}

static int contains_case_insensitive(const char *text, const char *needle)
{
	size_t i;
	size_t j;

	for (i = 0; text[i] != '\0'; i++)
	{
		j = 0;
		while (needle[j] != '\0' && text[i + j] != '\0'
			&& tolower((unsigned char)text[i + j])
				== tolower((unsigned char)needle[j]))
			j++;
		if (needle[j] == '\0')
			return (1);
	}
	return (0);
}

static void normalize_space(const char *source, char *out, size_t capacity)
{
	size_t used;
	int pending_space;

	used = 0;
	pending_space = 0;
	while (*source != '\0')
	{
		if (isspace((unsigned char)*source))
			pending_space = used > 0;
		else
		{
			if (pending_space)
			{
				assert(used + 1 < capacity);
				out[used++] = ' ';
			}
			assert(used + 1 < capacity);
			out[used++] = *source;
			pending_space = 0;
		}
		source++;
	}
	out[used] = '\0';
}

/* AC1: the first line is the exact required italic attribution. */
static void test_ac1_first_line(void)
{
	const char *expected;
	const char *end;

	expected = "*This project has been created as part of the 42 curriculum by sperez-l.*";
	end = strchr(g_readme, '\n');
	assert(end != NULL);
	assert((size_t)(end - g_readme) == strlen(expected));
	assert(strncmp(g_readme, expected, strlen(expected)) == 0);
}

/* AC2: each of the five mandatory section headings appears once. */
static void test_ac2_required_headings(void)
{
	static const char *names[] = {"Description", "Instructions", "Resources",
		"Blocking cases handled", "Thread synchronization mechanisms"};
	size_t i;

	for (i = 0; i < sizeof(names) / sizeof(names[0]); i++)
		assert(heading_count(names[i]) == 1);
}

/* AC3: the README has none of the four specified placeholder phrases. */
static void test_ac3_no_placeholders(void)
{
	static const char *phrases[] = {"At this stage", "nothing to block",
		"will describe", "will explain"};
	size_t i;

	for (i = 0; i < sizeof(phrases) / sizeof(phrases[0]); i++)
		assert(!contains_case_insensitive(g_readme, phrases[i]));
}

/* AC4: Blocking cases handled names all six required topics. */
static void test_ac4_blocking_topics(void)
{
	static const char *topics[] = {"deadlock", "Coffman", "starvation",
		"cooldown", "burnout", "log serialization"};
	char body[README_CAP];
	size_t i;

	section("Blocking cases handled", body, sizeof(body));
	for (i = 0; i < sizeof(topics) / sizeof(topics[0]); i++)
		assert(contains_case_insensitive(body, topics[i]));
}

/* AC5: Thread synchronization mechanisms names primitives and protected areas. */
static void test_ac5_synchronization_topics(void)
{
	static const char *topics[] = {"pthread_mutex_t", "pthread_cond_t",
		"dongles", "logging", "monitor state"};
	char body[README_CAP];
	size_t i;

	section("Thread synchronization mechanisms", body, sizeof(body));
	for (i = 0; i < sizeof(topics) / sizeof(topics[0]); i++)
		assert(contains_case_insensitive(body, topics[i]));
}

static void get_actual_usage(char *out, size_t capacity)
{
	const char *binary;
	FILE *error;
	pid_t child;
	size_t used;
	int status;

	binary = getenv("CODEXION_BIN");
	if (binary == NULL || *binary == '\0')
		binary = "coders/codexion";
	error = tmpfile();
	assert(error != NULL);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		if (dup2(fileno(error), STDERR_FILENO) < 0)
			_exit(126);
		execl(binary, binary, (char *)NULL);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
	assert(fseek(error, 0, SEEK_SET) == 0);
	used = fread(out, 1, capacity - 1, error);
	assert(!ferror(error));
	assert(used < capacity - 1 || fgetc(error) == EOF);
	out[used] = '\0';
	assert(fclose(error) == 0);
}

/* AC6: every README usage block equals the final binary's actual usage. */
static void test_ac6_usage_matches_binary(void)
{
	char actual[OUTPUT_CAP];
	const char *cursor;
	const char *line;
	int count;

	get_actual_usage(actual, sizeof(actual));
	assert(strncmp(actual, "Usage: ", 7) == 0);
	assert(strchr(actual, '\n') != NULL);
	cursor = g_readme;
	count = 0;
	while ((line = strstr(cursor, "Usage: ")) != NULL)
	{
		assert(line == g_readme || line[-1] == '\n');
		assert(strncmp(line, actual, strlen(actual)) == 0);
		cursor = line + strlen(actual);
		count++;
	}
	assert(count > 0);
}

/* AC7: Resources contains the user-approved AI paragraph verbatim, with line breaks normalized. */
static void test_ac7_ai_paragraph(void)
{
	static const char *paragraph = "**How AI was used.** I used this project to "
		"learn concurrency, and AI assistants were part of how I worked and "
		"learned. They helped break the work into small steps, drafted code "
		"and tests, and reviewed changes. I made the design decisions, "
		"approved each step, and used the process to study how threads, "
		"mutexes and scheduling work.";
	char body[README_CAP];
	char normalized[README_CAP];

	section("Resources", body, sizeof(body));
	normalize_space(body, normalized, sizeof(normalized));
	assert(strstr(normalized, paragraph) != NULL);
}

int main(void)
{
	read_readme();
	test_ac1_first_line();
	test_ac2_required_headings();
	test_ac3_no_placeholders();
	test_ac4_blocking_topics();
	test_ac5_synchronization_topics();
	test_ac6_usage_matches_binary();
	test_ac7_ai_paragraph();
	return (0);
}
