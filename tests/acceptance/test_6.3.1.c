#include "main.h"

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

typedef struct s_waiter
{
	t_sim			*sim;
	t_dongle		*dongle;
	pthread_t		thread;
	pthread_mutex_t	ready_mutex;
	pthread_cond_t	ready_cond;
	int				ready;
}	t_waiter;

static t_config get_config(int n)
{
	t_config config;

	config.number_of_coders = n;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
	return (config);
}

static void *wait_for_stop(void *arg)
{
	t_waiter *waiter;

	waiter = (t_waiter *)arg;
	assert(pthread_mutex_lock(&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_lock(&waiter->ready_mutex) == 0);
	waiter->ready = 1;
	assert(pthread_cond_signal(&waiter->ready_cond) == 0);
	assert(pthread_mutex_unlock(&waiter->ready_mutex) == 0);
	while (!is_stopped(waiter->sim))
		assert(pthread_cond_wait(&waiter->dongle->cond,
				&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->dongle->mutex) == 0);
	return (NULL);
}

static void start_waiter(t_waiter *waiter, t_sim *sim, int dongle_index)
{
	waiter->sim = sim;
	waiter->dongle = &sim->dongles[dongle_index];
	waiter->ready = 0;
	assert(pthread_mutex_init(&waiter->ready_mutex, NULL) == 0);
	assert(pthread_cond_init(&waiter->ready_cond, NULL) == 0);
	assert(pthread_create(&waiter->thread, NULL, wait_for_stop, waiter) == 0);
}

static void await_ready(t_waiter *waiter)
{
	assert(pthread_mutex_lock(&waiter->ready_mutex) == 0);
	while (!waiter->ready)
		assert(pthread_cond_wait(&waiter->ready_cond,
				&waiter->ready_mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->ready_mutex) == 0);
}

static void join_waiter(t_waiter *waiter)
{
	assert(pthread_join(waiter->thread, NULL) == 0);
	assert(pthread_cond_destroy(&waiter->ready_cond) == 0);
	assert(pthread_mutex_destroy(&waiter->ready_mutex) == 0);
}

static void await_cond_wait(t_dongle *dongle)
{
	assert(pthread_mutex_lock(&dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&dongle->mutex) == 0);
}

/* AC1: stopping wakes one waiter on each of four dongles. */
static void test_ac1_stop_wakes_all_dongles(void)
{
	t_config config = get_config(4);
	t_sim sim;
	t_waiter waiters[4];
	int i;

	assert(init_simulation(&sim, &config) == 0);
	for (i = 0; i < 4; i++)
		start_waiter(&waiters[i], &sim, i);
	for (i = 0; i < 4; i++)
		await_ready(&waiters[i]);
	for (i = 0; i < 4; i++)
		await_cond_wait(&sim.dongles[i]);
	assert(set_stopped(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	for (i = 0; i < 4; i++)
		join_waiter(&waiters[i]);
	assert(free_sim(&sim) == 0);
}

/* AC2: one broadcast wakes all three waiters on dongle zero. */
static void test_ac2_wakes_several_waiters(void)
{
	t_config config = get_config(4);
	t_sim sim;
	t_waiter waiters[3];
	int i;

	assert(init_simulation(&sim, &config) == 0);
	for (i = 0; i < 3; i++)
		start_waiter(&waiters[i], &sim, 0);
	for (i = 0; i < 3; i++)
		await_ready(&waiters[i]);
	await_cond_wait(&sim.dongles[0]);
	assert(set_stopped(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	for (i = 0; i < 3; i++)
		join_waiter(&waiters[i]);
	assert(free_sim(&sim) == 0);
}

/* AC3: 200 fresh stop-and-wake rounds lose no waiter wake-up. */
static void test_ac3_no_lost_wakeup(void)
{
	t_config config = get_config(4);
	t_sim sim;
	t_waiter waiter;
	int round;

	for (round = 0; round < 200; round++)
	{
		assert(init_simulation(&sim, &config) == 0);
		start_waiter(&waiter, &sim, 0);
		await_ready(&waiter);
		assert(set_stopped(&sim) == 0);
		assert(wake_dongles(&sim) == 0);
		join_waiter(&waiter);
		assert(free_sim(&sim) == 0);
	}
}

/* AC4: waking a running simulation preserves state and unlocks every dongle. */
static void test_ac4_preserves_state_and_unlocks(void)
{
	t_config config = get_config(4);
	t_sim sim;
	int i;

	assert(init_simulation(&sim, &config) == 0);
	for (i = 0; i < 4; i++)
	{
		sim.dongles[i].available = i % 2;
		sim.dongles[i].released_at = 100 + i;
		sim.dongles[i].heap.size = 1;
	}
	assert(wake_dongles(&sim) == 0);
	assert(is_stopped(&sim) == 0);
	for (i = 0; i < 4; i++)
	{
		assert(pthread_mutex_trylock(&sim.dongles[i].mutex) == 0);
		assert(sim.dongles[i].available == i % 2);
		assert(sim.dongles[i].released_at == 100 + i);
		assert(sim.dongles[i].heap.size == 1);
		assert(pthread_mutex_unlock(&sim.dongles[i].mutex) == 0);
	}
	assert(free_sim(&sim) == 0);
}

/* AC5: one coder has one dongle, whose waiter wakes after stop. */
static void test_ac5_one_coder_wakes(void)
{
	t_config config = get_config(1);
	t_sim sim;
	t_waiter waiter;

	assert(init_simulation(&sim, &config) == 0);
	start_waiter(&waiter, &sim, 0);
	await_ready(&waiter);
	await_cond_wait(&sim.dongles[0]);
	assert(set_stopped(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	join_waiter(&waiter);
	assert(free_sim(&sim) == 0);
}

/* AC6: null simulation or null dongle array is rejected safely. */
static void test_ac6_rejects_null_input(void)
{
	t_sim sim = {0};

	assert(wake_dongles(NULL) == -1);
	assert(wake_dongles(&sim) == -1);
}

int main(void)
{
	alarm(60);
	test_ac1_stop_wakes_all_dongles();
	test_ac2_wakes_several_waiters();
	test_ac3_no_lost_wakeup();
	test_ac4_preserves_state_and_unlocks();
	test_ac5_one_coder_wakes();
	test_ac6_rejects_null_input();
	alarm(0);
	return (0);
}
