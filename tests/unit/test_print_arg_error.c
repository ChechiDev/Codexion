/* Unit tests for print_arg_error: stderr capture of every position. */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

static void	capture_err(int position, char *dst, size_t cap)
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
	print_arg_error(position);
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
	static const char	*names[] = {"number_of_coders", "time_to_burnout",
		"time_to_compile", "time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown"};
	char				got[256];
	char				want[256];
	int					pos;

	pos = 1;
	while (pos <= 7)
	{
		capture_err(pos, got, sizeof(got));
		snprintf(want, sizeof(want), "Error: %s: expected an integer "
			"from %d to %d\n", names[pos - 1], get_arg_minimum(pos), INT_MAX);
		assert(strcmp(got, want) == 0);
		pos++;
	}
}

static void	test_scheduler_and_out_of_range(void)
{
	char	got[256];

	capture_err(8, got, sizeof(got));
	assert(strcmp(got, "Error: scheduler: expected fifo or edf\n") == 0);
	capture_err(0, got, sizeof(got));
	assert(got[0] == '\0');
	capture_err(9, got, sizeof(got));
	assert(got[0] == '\0');
	capture_err(-1, got, sizeof(got));
	assert(got[0] == '\0');
}

int	main(void)
{
	test_numeric_positions();
	test_scheduler_and_out_of_range();
	return (0);
}
