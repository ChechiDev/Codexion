/* Unit tests for print_arg_error: stderr capture of every position and kind. */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

static const char	*g_names[] = {"number_of_coders", "time_to_burnout",
	"time_to_compile", "time_to_debug", "time_to_refactor",
	"number_of_compiles_required", "dongle_cooldown"};

static void	capture_err(int position, int kind, char *dst, size_t cap)
{
	FILE	*tmp;
	int		saved;
	size_t	n;

	tmp = tmpfile();
	assert(tmp != NULL);
	fflush(stderr);
	saved = dup(STDERR_FILENO);
	assert(saved >= 0);
	assert(dup2(fileno(tmp), STDERR_FILENO) >= 0);
	print_arg_error(position, kind);
	fflush(stderr);
	assert(dup2(saved, STDERR_FILENO) >= 0);
	close(saved);
	rewind(tmp);
	n = fread(dst, 1, cap - 1, tmp);
	dst[n] = '\0';
	fclose(tmp);
}

static void	test_numeric_positions(void)
{
	char	got[256];
	char	want[256];
	int		pos;

	pos = 1;
	while (pos <= 7)
	{
		capture_err(pos, ARG_BAD_RANGE, got, sizeof(got));
		snprintf(want, sizeof(want), "Error: %s: expected an integer "
			"from %d to %d\n", g_names[pos - 1], get_arg_minimum(pos),
			INT_MAX);
		assert(strcmp(got, want) == 0);
		pos++;
	}
}

static void	test_sign_kind(void)
{
	char	got[256];
	char	want[256];
	int		pos;

	pos = 1;
	while (pos <= 7)
	{
		capture_err(pos, ARG_BAD_SIGN, got, sizeof(got));
		snprintf(want, sizeof(want), "Error: %s: invalid sign, use at most "
			"one '+' right before the digits\n", g_names[pos - 1]);
		assert(strcmp(got, want) == 0);
		pos++;
	}
	capture_err(8, ARG_BAD_SIGN, got, sizeof(got));
	assert(got[0] == '\0');
	capture_err(1, 0, got, sizeof(got));
	assert(got[0] == '\0');
	capture_err(1, 3, got, sizeof(got));
	assert(got[0] == '\0');
	capture_err(8, 0, got, sizeof(got));
	assert(got[0] == '\0');
}

static void	test_scheduler_and_out_of_range(void)
{
	char	got[256];

	capture_err(8, ARG_BAD_RANGE, got, sizeof(got));
	assert(strcmp(got, "Error: scheduler: expected fifo or edf\n") == 0);
	capture_err(0, ARG_BAD_RANGE, got, sizeof(got));
	assert(got[0] == '\0');
	capture_err(9, ARG_BAD_RANGE, got, sizeof(got));
	assert(got[0] == '\0');
	capture_err(-1, ARG_BAD_SIGN, got, sizeof(got));
	assert(got[0] == '\0');
	capture_err(9, ARG_BAD_SIGN, got, sizeof(got));
	assert(got[0] == '\0');
}

int	main(void)
{
	test_numeric_positions();
	test_sign_kind();
	test_scheduler_and_out_of_range();
	return (0);
}
