#include "main.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct s_inspection
{
	t_dongle	*first;
	t_dongle	*second;
	int		first_result;
	int		second_result;
} t_inspection;

typedef struct s_contender
{
	t_dongle		*dongle;
	pthread_mutex_t	ready_mutex;
	pthread_cond_t	ready_cond;
	int			ready;
	long			acquired_us;
	long			released_us;
} t_contender;

static t_config get_config(int count)
{
	t_config config;

	config.number_of_coders = count;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
	return (config);
}

static long get_wall_us(void)
{
	struct timeval now;

	assert(gettimeofday(&now, NULL) == 0);
	return (now.tv_sec * 1000000L + now.tv_usec);
}

static void lock_pair(t_sim *sim)
{
	assert(pthread_mutex_lock(&sim->dongles[0].mutex) == 0);
	assert(pthread_mutex_lock(&sim->dongles[1].mutex) == 0);
}

static void unlock_pair(t_sim *sim)
{
	assert(pthread_mutex_unlock(&sim->dongles[1].mutex) == 0);
	assert(pthread_mutex_unlock(&sim->dongles[0].mutex) == 0);
}

static void *inspect_pair(void *arg)
{
	t_inspection *inspection;

	inspection = (t_inspection *)arg;
	inspection->first_result = pthread_mutex_trylock(
			&inspection->first->mutex);
	if (inspection->first_result == 0)
		assert(pthread_mutex_unlock(&inspection->first->mutex) == 0);
	inspection->second_result = pthread_mutex_trylock(
			&inspection->second->mutex);
	if (inspection->second_result == 0)
		assert(pthread_mutex_unlock(&inspection->second->mutex) == 0);
	return (NULL);
}

static void assert_pair_held(t_sim *sim)
{
	t_inspection inspection;
	pthread_t thread;

	inspection.first = &sim->dongles[0];
	inspection.second = &sim->dongles[1];
	assert(pthread_create(&thread, NULL, inspect_pair, &inspection) == 0);
	assert(pthread_join(thread, NULL) == 0);
	assert(inspection.first_result == EBUSY);
	assert(inspection.second_result == EBUSY);
}

static void *hold_second_dongle(void *arg)
{
	t_contender *contender;

	contender = (t_contender *)arg;
	assert(pthread_mutex_lock(&contender->ready_mutex) == 0);
	contender->ready = 1;
	assert(pthread_cond_signal(&contender->ready_cond) == 0);
	assert(pthread_mutex_unlock(&contender->ready_mutex) == 0);
	assert(pthread_mutex_lock(&contender->dongle->mutex) == 0);
	contender->acquired_us = get_wall_us();
	assert(usleep(20000) == 0);
	contender->released_us = get_wall_us();
	assert(pthread_mutex_unlock(&contender->dongle->mutex) == 0);
	return (NULL);
}

static void await_contender(t_contender *contender)
{
	assert(pthread_mutex_lock(&contender->ready_mutex) == 0);
	while (!contender->ready)
		assert(pthread_cond_wait(&contender->ready_cond,
				&contender->ready_mutex) == 0);
	assert(pthread_mutex_unlock(&contender->ready_mutex) == 0);
}

/* AC1: an unsignalled wait lasts at least one millisecond, returns promptly,
 * and restores ownership of both dongle mutexes. */
static void test_ac1_timed_slice_restores_both_mutexes(void)
{
	t_config config;
	t_sim sim;
	long start_us;
	long elapsed_us;

	config = get_config(4);
	assert(init_simulation(&sim, &config) == 0);
	assert(sim.coders[0].first_dongle == 0);
	assert(sim.coders[0].second_dongle == 1);
	lock_pair(&sim);
	start_us = get_wall_us();
	assert(wait_for_dongles(&sim.coders[0]) == 0);
	elapsed_us = get_wall_us() - start_us;
	assert(elapsed_us >= 1000 && elapsed_us < 1000000);
	assert_pair_held(&sim);
	unlock_pair(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC2: a contender takes the second mutex during the wait and the waiter
 * cannot return until the contender's 20 ms hold has ended. */
static void test_ac2_second_mutex_is_free_during_wait(void)
{
	t_config config;
	t_sim sim;
	t_contender contender;
	pthread_t thread;
	long start_us;
	long returned_us;

	config = get_config(4);
	assert(init_simulation(&sim, &config) == 0);
	lock_pair(&sim);
	contender.dongle = &sim.dongles[1];
	contender.ready = 0;
	contender.acquired_us = 0;
	contender.released_us = 0;
	assert(pthread_mutex_init(&contender.ready_mutex, NULL) == 0);
	assert(pthread_cond_init(&contender.ready_cond, NULL) == 0);
	assert(pthread_create(&thread, NULL, hold_second_dongle,
			&contender) == 0);
	await_contender(&contender);
	assert(usleep(1000) == 0);
	start_us = get_wall_us();
	assert(wait_for_dongles(&sim.coders[0]) == 0);
	returned_us = get_wall_us();
	assert_pair_held(&sim);
	unlock_pair(&sim);
	assert(pthread_join(thread, NULL) == 0);
	assert(contender.acquired_us >= start_us);
	assert(contender.released_us >= contender.acquired_us + 20000);
	assert(returned_us >= contender.released_us);
	assert(returned_us - start_us >= 20000);
	assert(pthread_cond_destroy(&contender.ready_cond) == 0);
	assert(pthread_mutex_destroy(&contender.ready_mutex) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC3: 300 consecutive waits return successfully with both mutexes held;
 * the code-gate valgrind run checks for leaks. */
static void test_ac3_repeated_waits_preserve_ownership(void)
{
	t_config config;
	t_sim sim;
	int round;

	config = get_config(4);
	assert(init_simulation(&sim, &config) == 0);
	lock_pair(&sim);
	round = 0;
	while (round < 300)
	{
		assert(wait_for_dongles(&sim.coders[0]) == 0);
		assert_pair_held(&sim);
		round++;
	}
	unlock_pair(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC4: null inputs and a one-dongle coder fail without changing mutex
 * ownership. */
static void test_ac4_invalid_input_preserves_mutexes(void)
{
	t_config config;
	t_sim sim;
	t_sim single;
	t_coder invalid = {0};
	pthread_t thread;
	t_inspection inspection;

	assert(wait_for_dongles(NULL) == -1);
	assert(wait_for_dongles(&invalid) == -1);
	config = get_config(1);
	assert(init_simulation(&single, &config) == 0);
	assert(single.coders[0].first_dongle ==
		single.coders[0].second_dongle);
	assert(pthread_mutex_lock(&single.dongles[0].mutex) == 0);
	assert(wait_for_dongles(&single.coders[0]) == -1);
	inspection.first = &single.dongles[0];
	inspection.second = &single.dongles[0];
	assert(pthread_create(&thread, NULL, inspect_pair, &inspection) == 0);
	assert(pthread_join(thread, NULL) == 0);
	assert(inspection.first_result == EBUSY);
	assert(inspection.second_result == EBUSY);
	assert(pthread_mutex_unlock(&single.dongles[0].mutex) == 0);
	assert(free_sim(&single) == 0);
	config = get_config(4);
	assert(init_simulation(&sim, &config) == 0);
	lock_pair(&sim);
	invalid.first_dongle = sim.coders[0].first_dongle;
	invalid.second_dongle = sim.coders[0].second_dongle;
	invalid.sim = NULL;
	assert(wait_for_dongles(&invalid) == -1);
	assert_pair_held(&sim);
	unlock_pair(&sim);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	alarm(60);
	test_ac1_timed_slice_restores_both_mutexes();
	test_ac2_second_mutex_is_free_during_wait();
	test_ac3_repeated_waits_preserve_ownership();
	test_ac4_invalid_input_preserves_mutexes();
	alarm(0);
	return (0);
}
