/* Unit tests for print_usage and the argument-count branch of main.c.
** main.c is included with its entry point renamed, because the test build
** links every coders/ object except main.c. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define main codexion_main
#include "main.c"
#undef main

#define CAP_SIZE 1024

typedef struct s_capture
{
	char	out[CAP_SIZE];
	char	err[CAP_SIZE];
}	t_capture;

static void	read_back(FILE *tmp, char *dst)
{
	size_t	n;

	rewind(tmp);
	n = fread(dst, 1, CAP_SIZE - 1, tmp);
	dst[n] = '\0';
}

/* Runs fn with stdout and stderr redirected to temp files. */
static int	run_captured(int (*fn)(void), t_capture *cap)
{
	FILE	*fout;
	FILE	*ferr;
	int		saved_out;
	int		saved_err;
	int		ret;

	fout = tmpfile();
	ferr = tmpfile();
	assert(fout != NULL && ferr != NULL);
	fflush(stdout);
	fflush(stderr);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(fout), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(ferr), STDERR_FILENO) >= 0);
	ret = fn();
	fflush(stdout);
	fflush(stderr);
	assert(dup2(saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(saved_err, STDERR_FILENO) >= 0);
	close(saved_out);
	close(saved_err);
	read_back(fout, cap->out);
	read_back(ferr, cap->err);
	fclose(fout);
	fclose(ferr);
	return (ret);
}

static int	g_argc;

static int	call_main(void)
{
	char	*argv[16];
	int		i;

	i = 0;
	while (i < 16)
		argv[i++] = "x";
	return (codexion_main(g_argc, argv));
}

static int	call_usage(void)
{
	print_usage();
	return (0);
}

static void	check_usage_rejected(int argc)
{
	t_capture	cap;

	g_argc = argc;
	assert(run_captured(call_main, &cap) == EXIT_USAGE);
	assert(EXIT_USAGE == 1);
	assert(strncmp(cap.err, "Usage:", 6) == 0);
	assert(cap.out[0] == '\0');
}

static void	test_wrong_argument_counts(void)
{
	check_usage_rejected(1);
	check_usage_rejected(8);
	check_usage_rejected(10);
}

static void	test_exact_argument_count(void)
{
	t_capture	cap;

	g_argc = 9;
	assert(run_captured(call_main, &cap) == 0);
	assert(cap.err[0] == '\0');
	assert(cap.out[0] == '\0');
}

static void	test_usage_text(void)
{
	static const char	*names[] = {"number_of_coders", "time_to_burnout",
		"time_to_compile", "time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown", "scheduler"};
	t_capture			cap;
	const char			*pos;
	const char			*found;
	size_t				i;

	run_captured(call_usage, &cap);
	assert(strncmp(cap.err, "Usage:", 6) == 0);
	assert(cap.out[0] == '\0');
	pos = cap.err;
	i = 0;
	while (i < sizeof(names) / sizeof(names[0]))
	{
		found = strstr(pos, names[i]);
		assert(found != NULL);
		pos = found + strlen(names[i]);
		i++;
	}
	assert(strstr(cap.err, "fifo") != NULL);
	assert(strstr(cap.err, "edf") != NULL);
}

int	main(void)
{
	test_wrong_argument_counts();
	test_exact_argument_count();
	test_usage_text();
	puts("test_main_usage: OK");
	return (0);
}
