/* Unit tests for print_state: line format, rejections, lock failures. */

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

static void	init_sim(t_sim *sim, int errorcheck)
{
	pthread_mutexattr_t	attr;

	assert(pthread_mutexattr_init(&attr) == 0);
	if (errorcheck)
		assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	sim->start_ms = get_time_ms();
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->log_mutex, &attr) == 0);
	assert(pthread_mutex_init(&sim->stop_mutex, &attr) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
}

static void	destroy_sim(t_sim *sim)
{
	assert(pthread_mutex_trylock(&sim->log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->log_mutex) == 0);
	assert(pthread_mutex_trylock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
}

static int	redirect_stdout(int *fds)
{
	int	saved;

	assert(pipe(fds) == 0);
	assert(fcntl(fds[0], F_SETFL, O_NONBLOCK) == 0);
	saved = dup(STDOUT_FILENO);
	assert(saved >= 0 && dup2(fds[1], STDOUT_FILENO) >= 0);
	return (saved);
}

static ssize_t	restore_stdout(int saved, int *fds, char *buf)
{
	ssize_t	n;

	assert(dup2(saved, STDOUT_FILENO) >= 0);
	n = read(fds[0], buf, 63);
	if (n < 0)
		n = 0;
	buf[n] = '\0';
	assert(close(saved) == 0 && close(fds[0]) == 0 && close(fds[1]) == 0);
	return (n);
}

static void	test_all_states_text(void)
{
	static const char	*texts[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring"};
	t_sim				sim;
	int					fds[2];
	int					saved;
	char				buf[64];
	char				tail[48];
	int					state;

	state = 0;
	while (state < 4)
	{
		init_sim(&sim, 0);
		saved = redirect_stdout(fds);
		assert(print_state(&sim, 7, (t_state)state) == 0);
		assert(restore_stdout(saved, fds, buf) > 0);
		snprintf(tail, sizeof(tail), " 7 %s\n", texts[state]);
		assert(strstr(buf, tail) != NULL);
		assert(buf[0] >= '0' && buf[0] <= '9');
		destroy_sim(&sim);
		state++;
	}
}

static void	test_rejections_write_nothing(void)
{
	t_sim	sim;
	int		fds[2];
	int		saved;
	char	buf[64];

	init_sim(&sim, 0);
	saved = redirect_stdout(fds);
	assert(print_state(&sim, 1, (t_state)-1) == -1);
	assert(print_state(&sim, 1, (t_state)4) == -1);
	assert(print_state(&sim, 0, STATE_COMPILING) == -1);
	assert(print_state(&sim, -3, STATE_COMPILING) == -1);
	assert(set_stopped(&sim) == 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == -1);
	assert(restore_stdout(saved, fds, buf) == 0);
	destroy_sim(&sim);
}

static void	test_lock_failures(void)
{
	t_sim	sim;
	int		fds[2];
	int		saved;
	char	buf[64];

	init_sim(&sim, 1);
	saved = redirect_stdout(fds);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == -1);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == -1);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(restore_stdout(saved, fds, buf) == 0);
	assert(sim.stopped == 0);
	destroy_sim(&sim);
}

static void	test_write_error(void)
{
	t_sim	sim;
	int		saved;
	int		full;

	init_sim(&sim, 0);
	full = open("/dev/full", O_WRONLY);
	assert(full >= 0);
	saved = dup(STDOUT_FILENO);
	assert(saved >= 0 && dup2(full, STDOUT_FILENO) >= 0);
	assert(print_state(&sim, 1, STATE_COMPILING) == -1);
	assert(dup2(saved, STDOUT_FILENO) >= 0);
	assert(close(saved) == 0 && close(full) == 0);
	assert(sim.stopped == 0);
	destroy_sim(&sim);
}

int	main(void)
{
	test_all_states_text();
	test_rejections_write_nothing();
	test_lock_failures();
	test_write_error();
	return (0);
}
