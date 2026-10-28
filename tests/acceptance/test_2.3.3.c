#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

static void valid_args(char **argv)
{
	argv[0] = "codexion";
	argv[1] = "4";
	argv[2] = "800";
	argv[3] = "200";
	argv[4] = "200";
	argv[5] = "200";
	argv[6] = "3";
	argv[7] = "50";
	argv[8] = "edf";
	argv[9] = NULL;
}

/* AC1: a valid edf invocation fills every config field. */
static void test_ac1_valid_edf_config(void)
{
	char *argv[10];
	t_config config;

	valid_args(argv);
	assert(parse_config(argv, &config) == 0);
	assert(config.number_of_coders == 4);
	assert(config.time_to_burnout == 800);
	assert(config.time_to_compile == 200);
	assert(config.time_to_debug == 200);
	assert(config.time_to_refactor == 200);
	assert(config.number_of_compiles_required == 3);
	assert(config.dongle_cooldown == 50);
	assert(config.mode == MODE_EDF);
}

/* AC2: each numeric minimum is accepted with fifo. */
static void test_ac2_minimum_fifo_config(void)
{
	char *argv[10];
	t_config config;

	valid_args(argv);
	argv[1] = "1";
	argv[2] = "1";
	argv[3] = "0";
	argv[4] = "0";
	argv[5] = "0";
	argv[6] = "0";
	argv[7] = "0";
	argv[8] = "fifo";
	assert(parse_config(argv, &config) == 0);
	assert(config.number_of_coders == 1);
	assert(config.time_to_burnout == 1);
	assert(config.time_to_compile == 0);
	assert(config.time_to_debug == 0);
	assert(config.time_to_refactor == 0);
	assert(config.number_of_compiles_required == 0);
	assert(config.dongle_cooldown == 0);
	assert(config.mode == MODE_FIFO);
}

/* AC3: leading zeros and surrounding whitespace are accepted. */
static void test_ac3_trimmed_number(void)
{
	char *argv[10];
	t_config config;

	valid_args(argv);
	argv[1] = "  0007  ";
	assert(parse_config(argv, &config) == 0);
	assert(config.number_of_coders == 7);
}

/* AC4: the first two fields reject zero at their own positions. */
static void test_ac4_positive_field_minimums(void)
{
	char *argv[10];
	t_config config;

	valid_args(argv);
	argv[1] = "0";
	assert(parse_config(argv, &config) == (1 << 1));
	valid_args(argv);
	argv[2] = "0";
	assert(parse_config(argv, &config) == (1 << 2));
}

/* AC5: every numeric position rejects each invalid number alone. */
static void test_ac5_invalid_numeric_positions(void)
{
	static const char *invalid[] = {"-1", "abc", "", "2147483648"};
	char *argv[10];
	t_config config;
	int position;
	size_t index;

	position = 1;
	while (position <= 7)
	{
		index = 0;
		while (index < sizeof(invalid) / sizeof(invalid[0]))
		{
			valid_args(argv);
			argv[position] = (char *)invalid[index];
			assert(parse_config(argv, &config) == (1 << position));
			index++;
		}
		position++;
	}
}

/* AC6: invalid schedulers set only bit eight. */
static void test_ac6_invalid_scheduler(void)
{
	static const char *invalid[] = {"FIFO", " edf", "rr"};
	char *argv[10];
	t_config config;
	size_t index;

	index = 0;
	while (index < sizeof(invalid) / sizeof(invalid[0]))
	{
		valid_args(argv);
		argv[8] = (char *)invalid[index];
		assert(parse_config(argv, &config) == (1 << 8));
		index++;
	}
}

/* AC7: independent errors combine without setting bit zero. */
static void test_ac7_all_invalid_positions_reported(void)
{
	char *argv[10];
	t_config config;
	int position;

	valid_args(argv);
	argv[2] = "abc";
	argv[3] = "-1";
	argv[8] = "rr";
	assert(parse_config(argv, &config) == 0x10c);
	valid_args(argv);
	position = 1;
	while (position <= 7)
		argv[position++] = "abc";
	argv[8] = "rr";
	assert(parse_config(argv, &config) == 0x1fe);
}

/* AC8: parsing does not alter any argv string, including invalid strings. */
static void test_ac8_argv_unchanged(void)
{
	char arg0[] = "codexion";
	char arg1[] = "  0007  ";
	char arg2[] = "abc";
	char arg3[] = "200";
	char arg4[] = "200";
	char arg5[] = "200";
	char arg6[] = "3";
	char arg7[] = "50";
	char arg8[] = " edf";
	char *argv[] = {arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, NULL};
	const char *original[] = {"codexion", "  0007  ", "abc", "200", "200",
		"200", "3", "50", " edf"};
	t_config config;
	int position;

	assert(parse_config(argv, &config) == ((1 << 2) | (1 << 8)));
	position = 0;
	while (position <= 8)
	{
		assert(strlen(argv[position]) == strlen(original[position]));
		assert(memcmp(argv[position], original[position],
			strlen(original[position]) + 1) == 0);
		position++;
	}
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC9: the deliverable builds with the parser and passes Norm. */
static void test_ac9_build_and_norm(void)
{
	char text[4096];
	FILE *file;
	size_t used;
	struct stat info;

	file = fopen("coders/Makefile", "r");
	assert(file != NULL);
	used = fread(text, 1, sizeof(text) - 1, file);
	assert(!ferror(file));
	assert(fclose(file) == 0);
	text[used] = '\0';
	assert(strstr(text, "src/parsing/parse_config.c") != NULL);
	run_command("make -C coders >/dev/null 2>&1");
	assert(stat("coders/codexion", &info) == 0);
	assert(S_ISREG(info.st_mode));
	assert((info.st_mode & 0111) != 0);
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_valid_edf_config();
	test_ac2_minimum_fifo_config();
	test_ac3_trimmed_number();
	test_ac4_positive_field_minimums();
	test_ac5_invalid_numeric_positions();
	test_ac6_invalid_scheduler();
	test_ac7_all_invalid_positions_reported();
	test_ac8_argv_unchanged();
	test_ac9_build_and_norm();
	return (0);
}
