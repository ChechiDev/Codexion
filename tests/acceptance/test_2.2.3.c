#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define HEADER_CAP 16384

static void read_header(char *text)
{
	FILE	*file;
	size_t	used;

	file = fopen("coders/main.h", "r");
	assert(file != NULL);
	used = fread(text, 1, HEADER_CAP - 1, file);
	assert(!ferror(file));
	assert(used < HEADER_CAP - 1 || fgetc(file) == EOF);
	text[used] = '\0';
	assert(fclose(file) == 0);
}

static const char *section(const char *text, const char *title)
{
	const char	*found;

	found = strstr(text, title);
	assert(found != NULL);
	assert(strstr(found + strlen(title), title) == NULL);
	return (found);
}

static void assert_between(const char *start, const char *end,
	const char *declaration)
{
	const char	*found;

	found = strstr(start, declaration);
	assert(found != NULL && found < end);
}

/* AC1: the five unique titles occur in order inside the include guard. */
static void test_ac1_ordered_sections(void)
{
	char		text[HEADER_CAP];
	const char	*macros;
	const char	*types;
	const char	*utils;
	const char	*parsing;
	const char	*cli;
	const char	*guard_end;

	read_header(text);
	macros = section(text, "/* ---- Macros ---- */");
	types = section(text, "/* ---- Types ---- */");
	utils = section(text, "/* ---- Utils ---- */");
	parsing = section(text, "/* ---- Parsing ---- */");
	cli = section(text, "/* ---- CLI ---- */");
	guard_end = strstr(text, "#endif");
	assert_between(text, macros, "#ifndef MAIN_H");
	assert_between(text, macros, "# define MAIN_H");
	assert(guard_end != NULL && cli < guard_end);
	assert(macros < types && types < utils);
	assert(utils < parsing && parsing < cli);
}

/* AC2: each existing declaration appears under its assigned title. */
static void test_ac2_declarations_in_sections(void)
{
	char		text[HEADER_CAP];
	const char	*macros;
	const char	*types;
	const char	*utils;
	const char	*parsing;
	const char	*cli;
	const char	*end;

	read_header(text);
	macros = section(text, "/* ---- Macros ---- */");
	types = section(text, "/* ---- Types ---- */");
	utils = section(text, "/* ---- Utils ---- */");
	parsing = section(text, "/* ---- Parsing ---- */");
	cli = section(text, "/* ---- CLI ---- */");
	end = strstr(cli, "#endif");
	assert_between(macros, types, "# define EXIT_USAGE");
	assert_between(types, utils, "typedef enum e_mode");
	assert_between(types, utils, "typedef struct s_config");
	assert((strstr(utils, "trim_span(") != NULL
		&& strstr(utils, "trim_span(") < parsing)
		|| (strstr(utils, "trim_slice(") != NULL
			&& strstr(utils, "trim_slice(") < parsing));
	assert_between(parsing, cli, "parse_number(");
	assert(end != NULL);
	assert_between(cli, end, "print_usage(");
}

/* AC3: stable public types, values, fields and function signatures work. */
static void test_ac3_stable_public_api(void)
{
	t_config	config;
	t_mode	mode;
	int		(*number_parser)(const char *, int *);
	void	(*usage_printer)(void);
	int		value;
	FILE	*errors;
	int		saved;

	assert(EXIT_USAGE == 1);
	mode = MODE_FIFO;
	assert(mode != MODE_EDF);
	assert(_Generic(config.number_of_coders, int: 1, default: 0));
	assert(_Generic(config.time_to_burnout, int: 1, default: 0));
	assert(_Generic(config.time_to_compile, int: 1, default: 0));
	assert(_Generic(config.time_to_debug, int: 1, default: 0));
	assert(_Generic(config.time_to_refactor, int: 1, default: 0));
	assert(_Generic(config.number_of_compiles_required, int: 1, default: 0));
	assert(_Generic(config.dongle_cooldown, int: 1, default: 0));
	assert(_Generic(config.mode, t_mode: 1, default: 0));
	number_parser = parse_number;
	usage_printer = print_usage;
	value = -1;
	assert(number_parser("42", &value) == 0 && value == 42);
	errors = tmpfile();
	assert(errors != NULL);
	saved = dup(fileno(stderr));
	assert(saved >= 0 && dup2(fileno(errors), fileno(stderr)) >= 0);
	usage_printer();
	assert(fflush(stderr) == 0);
	assert(dup2(saved, fileno(stderr)) >= 0 && close(saved) == 0);
	assert(fseek(errors, 0, SEEK_SET) == 0);
	assert(fgetc(errors) != EOF);
	assert(fclose(errors) == 0);
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

/* AC4: the header has no system includes; the deliverable builds and passes Norm. */
static void test_ac4_header_build_and_norm(void)
{
	char		text[HEADER_CAP];
	const char	*line;

	read_header(text);
	line = text;
	while (*line != '\0')
	{
		while (*line == ' ' || *line == '\t')
			line++;
		assert(strncmp(line, "#include <", 10) != 0);
		assert(strncmp(line, "# include <", 11) != 0);
		line = strchr(line, '\n');
		if (line == NULL)
			break ;
		line++;
	}
	run_command("make -C coders 2>&1");
	run_command("norminette coders/ 2>&1");
}

int main(void)
{
	test_ac1_ordered_sections();
	test_ac2_declarations_in_sections();
	test_ac3_stable_public_api();
	test_ac4_header_build_and_norm();
	return (0);
}
