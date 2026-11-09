/* Unit tests for print_run_error: fixed stderr line, nothing on stdout. */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

static void	capture_streams(int calls, char *out, char *err, size_t cap)
{
	FILE	*tmp_out;
	FILE	*tmp_err;
	int		saved[2];
	size_t	n;

	tmp_out = tmpfile();
	tmp_err = tmpfile();
	assert(tmp_out != NULL && tmp_err != NULL);
	fflush(stdout);
	fflush(stderr);
	saved[0] = dup(STDOUT_FILENO);
	saved[1] = dup(STDERR_FILENO);
	assert(saved[0] >= 0 && saved[1] >= 0);
	assert(dup2(fileno(tmp_out), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(tmp_err), STDERR_FILENO) >= 0);
	while (calls-- > 0)
		print_run_error();
	fflush(stdout);
	fflush(stderr);
	assert(dup2(saved[0], STDOUT_FILENO) >= 0);
	assert(dup2(saved[1], STDERR_FILENO) >= 0);
	close(saved[0]);
	close(saved[1]);
	rewind(tmp_out);
	rewind(tmp_err);
	n = fread(out, 1, cap - 1, tmp_out);
	out[n] = '\0';
	n = fread(err, 1, cap - 1, tmp_err);
	err[n] = '\0';
	fclose(tmp_out);
	fclose(tmp_err);
}

static void	test_single_call(void)
{
	char	out[256];
	char	err[256];

	capture_streams(1, out, err, sizeof(out));
	assert(strcmp(err, "Error: simulation failed\n") == 0);
	assert(out[0] == '\0');
}

static void	test_repeated_calls(void)
{
	char	out[256];
	char	err[256];

	capture_streams(3, out, err, sizeof(out));
	assert(strcmp(err, "Error: simulation failed\n"
			"Error: simulation failed\n"
			"Error: simulation failed\n") == 0);
	assert(out[0] == '\0');
}

int	main(void)
{
	test_single_call();
	test_repeated_calls();
	return (0);
}
