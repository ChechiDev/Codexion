#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUTPUT_CAP 8192
#define README_CAP 65536

typedef struct s_run
{
	int exit_code;
	char out[OUTPUT_CAP];
	char err[OUTPUT_CAP];
} t_run;

static void read_stream(FILE *stream, char *buffer, size_t capacity)
{
	size_t used;

	assert(fseek(stream, 0, SEEK_SET) == 0);
	used = fread(buffer, 1, capacity - 1, stream);
	assert(!ferror(stream));
	assert(used < capacity - 1 || fgetc(stream) == EOF);
	buffer[used] = '\0';
}

static t_run run_with_argument_count(int count)
{
	const char *binary;
	char *arguments[11];
	FILE *out;
	FILE *err;
	pid_t child;
	int status;
	int i;
	t_run result;

	binary = getenv("CODEXION_BIN");
	if (binary == NULL || binary[0] == '\0')
		binary = "coders/codexion";
	arguments[0] = (char *)binary;
	i = 1;
	while (i <= count)
	{
		arguments[i] = (char *)"1";
		i++;
	}
	arguments[i] = NULL;
	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		if (dup2(fileno(out), STDOUT_FILENO) < 0
			|| dup2(fileno(err), STDERR_FILENO) < 0)
			_exit(126);
		execv(binary, arguments);
		_exit(127);
	}
	assert(waitpid(child, &status, 0) == child);
	assert(WIFEXITED(status));
	result.exit_code = WEXITSTATUS(status);
	read_stream(out, result.out, sizeof(result.out));
	read_stream(err, result.err, sizeof(result.err));
	assert(fclose(out) == 0);
	assert(fclose(err) == 0);
	return (result);
}

/* AC1: no arguments yield usage on stderr only and exit status 1. */
static void test_ac1_no_arguments(void)
{
	t_run run;

	run = run_with_argument_count(0);
	assert(run.exit_code == 1);
	assert(run.out[0] == '\0');
	assert(run.err[0] != '\0');
}

/* AC2: seven and nine arguments have the same rejection as no arguments. */
static void test_ac2_wrong_argument_counts(void)
{
	t_run baseline;
	t_run seven;
	t_run nine;

	baseline = run_with_argument_count(0);
	seven = run_with_argument_count(7);
	nine = run_with_argument_count(9);
	assert(seven.exit_code == 1 && nine.exit_code == 1);
	assert(seven.out[0] == '\0' && nine.out[0] == '\0');
	assert(strcmp(seven.err, baseline.err) == 0);
	assert(strcmp(nine.err, baseline.err) == 0);
}

/* AC3: usage names all eight arguments in order and lists fifo and edf. */
static void test_ac3_usage_content(void)
{
	static const char *names[] = {
		"number_of_coders", "time_to_burnout", "time_to_compile",
		"time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown", "scheduler"
	};
	t_run run;
	const char *cursor;
	size_t i;

	run = run_with_argument_count(0);
	assert(strncmp(run.err, "Usage:", strlen("Usage:")) == 0);
	cursor = run.err;
	i = 0;
	while (i < sizeof(names) / sizeof(names[0]))
	{
		cursor = strstr(cursor, names[i]);
		assert(cursor != NULL);
		cursor += strlen(names[i]);
		i++;
	}
	assert(strstr(cursor, "fifo") != NULL);
	assert(strstr(cursor, "edf") != NULL);
}

/* AC4: norminette reports no error in the deliverable directory. */
static void test_ac4_norminette_clean(void)
{
	FILE *norm;
	char output[OUTPUT_CAP];
	size_t used;
	size_t count;
	int status;

	norm = popen("norminette coders/ 2>&1", "r");
	assert(norm != NULL);
	used = 0;
	while (used < sizeof(output) - 1 && (count = fread(output + used, 1,
		sizeof(output) - 1 - used, norm)) != 0)
		used += count;
	output[used] = '\0';
	assert(feof(norm));
	status = pclose(norm);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
	assert(strstr(output, "Error") == NULL);
}

static int has_argument_explanation(const char *instructions, const char *name)
{
	const char *line;
	const char *end;
	const char *found;
	const char *separator;

	line = instructions;
	while (*line != '\0')
	{
		end = strchr(line, '\n');
		if (end == NULL)
			end = line + strlen(line);
		found = strstr(line, name);
		if (found != NULL && found < end)
		{
			separator = found + strlen(name);
			while (separator < end && (*separator == '`' || *separator == ' '))
				separator++;
			if (separator < end && (*separator == ':' || *separator == '-'))
			{
				separator++;
				while (separator < end && *separator == ' ')
					separator++;
				if (end - separator >= 8)
					return (1);
			}
		}
		line = *end == '\0' ? end : end + 1;
	}
	return (0);
}

static int has_command_with_count(const char *instructions,
	const char *limit, int target)
{
	const char *command;
	const char *line;
	const char *end;
	const char *cursor;
	int count;

	command = instructions;
	while ((command = strstr(command, "./coders/codexion")) != NULL
		&& command < limit)
	{
		line = command;
		while (line > instructions && line[-1] != '\n')
			line--;
		while (line < command && (*line == ' ' || *line == '\t'
				|| *line == '$' || *line == '`'))
			line++;
		end = strchr(command, '\n');
		if (end == NULL)
			end = command + strlen(command);
		if (line != command)
		{
			command = end;
			continue;
		}
		cursor = command + strlen("./coders/codexion");
		count = 0;
		while (cursor < end)
		{
			while (cursor < end && (*cursor == ' ' || *cursor == '\t'))
				cursor++;
			if (cursor == end || *cursor == '`')
				break;
			count++;
			while (cursor < end && *cursor != ' ' && *cursor != '\t')
				cursor++;
		}
		if (count == target)
			return (1);
		command = end;
	}
	return (0);
}

/* AC5: Instructions give the run command, explain each input, and show the exact usage output. */
static void test_ac5_readme_instructions(void)
{
	static const char *names[] = {
		"number_of_coders", "time_to_burnout", "time_to_compile",
		"time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown", "scheduler"
	};
	char readme[README_CAP];
	char *instructions;
	char *next_section;
	FILE *file;
	size_t used;
	size_t i;
	t_run run;

	file = fopen("README.md", "r");
	assert(file != NULL);
	used = fread(readme, 1, sizeof(readme) - 1, file);
	assert(!ferror(file));
	assert(used < sizeof(readme) - 1 || fgetc(file) == EOF);
	readme[used] = '\0';
	assert(fclose(file) == 0);
	instructions = strstr(readme, "## Instructions");
	assert(instructions != NULL);
	next_section = strstr(instructions + strlen("## Instructions"), "\n## ");
	assert(next_section != NULL);
	*next_section = '\0';
	assert(strstr(instructions, "./coders/codexion") != NULL);
	i = 0;
	while (i < sizeof(names) / sizeof(names[0]))
	{
		assert(has_argument_explanation(instructions, names[i]));
		i++;
	}
	run = run_with_argument_count(0);
	assert(run.err[0] != '\0');
	next_section = strstr(instructions, run.err);
	assert(next_section != NULL);
	assert(has_command_with_count(instructions,
		instructions + strlen(instructions), 8));
	assert(has_command_with_count(instructions, next_section, 0)
		|| has_command_with_count(instructions, next_section, 7)
		|| has_command_with_count(instructions, next_section, 9));
}

int main(void)
{
	test_ac1_no_arguments();
	test_ac2_wrong_argument_counts();
	test_ac3_usage_content();
	test_ac4_norminette_clean();
	test_ac5_readme_instructions();
	return (0);
}
