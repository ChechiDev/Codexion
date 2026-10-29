/* Unit tests for print_burnout: first stop, line, already stopped, bad id. */

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

static void	init_sim(t_sim *sim)
{
	pthread_mutexattr_t	attr;

	memset(sim, 0, sizeof(*sim));
	assert(pthread_mutexattr_init(&attr) == 0);
	assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	sim->start_ms = get_time_ms();
	assert(pthread_mutex_init(&sim->log_mutex, &attr) == 0);
	assert(pthread_mutex_init(&sim->stop_mutex, &attr) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
}

static void	destroy_sim(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
}

static void	check_unlocked(t_sim *sim)
{
	assert(pthread_mutex_trylock(&sim->log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->log_mutex) == 0);
	assert(pthread_mutex_trylock(&sim->stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->stop_mutex) == 0);
}

static void	test_first_and_second_call(void)
{
	t_sim	sim;
	int		fds[2];
	int		saved;
	char	buf[64];
	ssize_t	n;

	init_sim(&sim);
	assert(pipe(fds) == 0 && fcntl(fds[0], F_SETFL, O_NONBLOCK) == 0);
	saved = dup(STDOUT_FILENO);
	assert(dup2(fds[1], STDOUT_FILENO) >= 0);
	assert(print_burnout(&sim, 4) == 0);
	assert(sim.stopped == 1);
	assert(print_burnout(&sim, 5) == -1);
	assert(dup2(saved, STDOUT_FILENO) >= 0);
	n = read(fds[0], buf, 63);
	assert(n > 0);
	buf[n] = '\0';
	assert(strstr(buf, " 4 burned out\n") != NULL);
	assert(strchr(buf, '\n') == buf + n - 1);
	check_unlocked(&sim);
	destroy_sim(&sim);
	close(saved);
	close(fds[0]);
	close(fds[1]);
}

static void	test_bad_coder(void)
{
	t_sim	sim;

	init_sim(&sim);
	assert(print_burnout(&sim, 0) == -1);
	assert(sim.stopped == 1);
	check_unlocked(&sim);
	destroy_sim(&sim);
}

int	main(void)
{
	test_first_and_second_call();
	test_bad_coder();
	return (0);
}
