/* Unit tests for print_log_line: one line, rejections, no lock, no stop check. */

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

static void	test_line_written(void)
{
	t_sim	sim;
	int		fds[2];
	int		saved;
	char	buf[64];
	ssize_t	n;

	memset(&sim, 0, sizeof(sim));
	sim.start_ms = get_time_ms();
	assert(pipe(fds) == 0 && fcntl(fds[0], F_SETFL, O_NONBLOCK) == 0);
	saved = dup(STDOUT_FILENO);
	assert(dup2(fds[1], STDOUT_FILENO) >= 0);
	assert(print_log_line(&sim, 7, "is compiling") == 0);
	assert(print_log_line(&sim, 0, "is compiling") == -1);
	assert(print_log_line(&sim, 3, "") == -1);
	assert(print_log_line(&sim, 3, "a\nb") == -1);
	assert(dup2(saved, STDOUT_FILENO) >= 0);
	n = read(fds[0], buf, 63);
	assert(n > 0);
	buf[n] = '\0';
	assert(strstr(buf, " 7 is compiling\n") != NULL);
	assert(buf[n - 1] == '\n' && strchr(buf, '\n') == buf + n - 1);
	close(saved);
	close(fds[0]);
	close(fds[1]);
}

int	main(void)
{
	test_line_written();
	return (0);
}
