#include "main.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

int release_dongles(t_coder *coder);

typedef struct s_waiter
{
	t_dongle        *dongle;
	pthread_t       thread;
	pthread_mutex_t ready_mutex;
	pthread_cond_t  ready_cond;
	int             ready;
} t_waiter;

typedef struct s_release_call
{
	t_coder  *coder;
	int      result;
	pthread_t thread;
} t_release_call;

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

static void set_taken(t_dongle *dongle)
{
	assert(pthread_mutex_lock(&dongle->mutex) == 0);
	dongle->available = 0;
	assert(pthread_mutex_unlock(&dongle->mutex) == 0);
}

static void assert_unlocked(t_dongle *dongle)
{
	assert(pthread_mutex_trylock(&dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&dongle->mutex) == 0);
}

static void *wait_for_available(void *arg)
{
	t_waiter *waiter = (t_waiter *)arg;

	assert(pthread_mutex_lock(&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_lock(&waiter->ready_mutex) == 0);
	waiter->ready = 1;
	assert(pthread_cond_signal(&waiter->ready_cond) == 0);
	assert(pthread_mutex_unlock(&waiter->ready_mutex) == 0);
	while (waiter->dongle->available == 0)
		assert(pthread_cond_wait(&waiter->dongle->cond,
				&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->dongle->mutex) == 0);
	return (NULL);
}

static void start_waiter(t_waiter *waiter, t_dongle *dongle)
{
	waiter->dongle = dongle;
	waiter->ready = 0;
	assert(pthread_mutex_init(&waiter->ready_mutex, NULL) == 0);
	assert(pthread_cond_init(&waiter->ready_cond, NULL) == 0);
	assert(pthread_create(&waiter->thread, NULL, wait_for_available,
			waiter) == 0);
}

static void await_waiter(t_waiter *waiter)
{
	assert(pthread_mutex_lock(&waiter->ready_mutex) == 0);
	while (!waiter->ready)
		assert(pthread_cond_wait(&waiter->ready_cond,
				&waiter->ready_mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->ready_mutex) == 0);
	assert(pthread_mutex_lock(&waiter->dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&waiter->dongle->mutex) == 0);
}

static void join_waiter(t_waiter *waiter)
{
	assert(pthread_join(waiter->thread, NULL) == 0);
	assert(pthread_cond_destroy(&waiter->ready_cond) == 0);
	assert(pthread_mutex_destroy(&waiter->ready_mutex) == 0);
}

static void *call_release(void *arg)
{
	t_release_call *call = (t_release_call *)arg;

	call->result = release_dongles(call->coder);
	return (NULL);
}

/* AC1: release updates only the two held dongles at the current simulation time. */
static void test_ac1_releases_pair_at_current_time(void)
{
	t_config config = get_config(4);
	t_sim sim;
	long before;
	long after;
	int i;

	assert(init_simulation(&sim, &config) == 0);
	assert(sim.coders[0].first_dongle == 0);
	assert(sim.coders[0].second_dongle == 1);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	set_taken(&sim.dongles[0]);
	set_taken(&sim.dongles[1]);
	before = get_elapsed_ms(sim.start_ms);
	assert(before >= 0);
	assert(release_dongles(&sim.coders[0]) == 0);
	after = get_elapsed_ms(sim.start_ms);
	assert(after >= before);
	for (i = 0; i < 4; i++)
	{
		assert_unlocked(&sim.dongles[i]);
		assert(sim.dongles[i].available == 1);
		if (i < 2)
			assert(sim.dongles[i].released_at >= before
				&& sim.dongles[i].released_at <= after);
		else
			assert(sim.dongles[i].released_at == -1);
	}
	assert(free_sim(&sim) == 0);
}

/* AC2: release wakes a waiter on each of the two dongle conditions. */
static void test_ac2_wakes_both_waiters(void)
{
	t_config config = get_config(4);
	t_sim sim;
	t_waiter first;
	t_waiter second;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	set_taken(&sim.dongles[0]);
	set_taken(&sim.dongles[1]);
	start_waiter(&first, &sim.dongles[0]);
	start_waiter(&second, &sim.dongles[1]);
	await_waiter(&first);
	await_waiter(&second);
	assert(release_dongles(&sim.coders[0]) == 0);
	join_waiter(&first);
	join_waiter(&second);
	assert(free_sim(&sim) == 0);
}

/* AC3: release preserves queued requests and starts the cooldown at release. */
static void test_ac3_preserves_queue_and_cooldown(void)
{
	t_config config = get_config(4);
	t_sim sim;
	t_request request = {17, 3, 2};
	t_request actual;
	long released_at;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	set_taken(&sim.dongles[0]);
	set_taken(&sim.dongles[1]);
	assert(push_request(&sim.dongles[1].heap, &request) == 0);
	assert(release_dongles(&sim.coders[0]) == 0);
	assert(sim.dongles[1].heap.size == 1);
	assert(get_head_request(&sim.dongles[1].heap, &actual) == 0);
	assert(actual.key == request.key);
	assert(actual.ticket == request.ticket);
	assert(actual.coder == request.coder);
	released_at = sim.dongles[1].released_at;
	assert(is_dongle_ready(&sim.dongles[1], 2, released_at + 49, 50) == 0);
	assert(is_dongle_ready(&sim.dongles[1], 2, released_at + 50, 50) == 1);
	assert(free_sim(&sim) == 0);
}

/* AC4: the one-coder path releases one dongle once and wakes its waiter. */
static void test_ac4_releases_single_dongle(void)
{
	t_config config = get_config(1);
	t_sim sim;
	t_waiter waiter;
	long before;
	long after;

	assert(init_simulation(&sim, &config) == 0);
	assert(sim.coders[0].first_dongle == 0);
	assert(sim.coders[0].second_dongle == 0);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	set_taken(&sim.dongles[0]);
	start_waiter(&waiter, &sim.dongles[0]);
	await_waiter(&waiter);
	before = get_elapsed_ms(sim.start_ms);
	assert(release_dongles(&sim.coders[0]) == 0);
	after = get_elapsed_ms(sim.start_ms);
	join_waiter(&waiter);
	assert(sim.dongles[0].available == 1);
	assert(sim.dongles[0].released_at >= before);
	assert(sim.dongles[0].released_at <= after);
	assert_unlocked(&sim.dongles[0]);
	assert(free_sim(&sim) == 0);
}

/* AC5: null coder, simulation, or dongle array changes no dongle state. */
static void test_ac5_rejects_invalid_input_unchanged(void)
{
	t_config config = get_config(4);
	t_sim sim;
	t_coder invalid = {0};
	t_dongle *saved;
	int i;

	assert(init_simulation(&sim, &config) == 0);
	for (i = 0; i < 4; i++)
	{
		sim.dongles[i].available = i % 2;
		sim.dongles[i].released_at = 100 + i;
	}
	assert(release_dongles(NULL) == -1);
	assert(release_dongles(&invalid) == -1);
	saved = sim.dongles;
	sim.dongles = NULL;
	assert(release_dongles(&sim.coders[0]) == -1);
	sim.dongles = saved;
	for (i = 0; i < 4; i++)
	{
		assert(sim.dongles[i].available == i % 2);
		assert(sim.dongles[i].released_at == 100 + i);
		assert_unlocked(&sim.dongles[i]);
	}
	assert(free_sim(&sim) == 0);
}

/* AC6: contention on the second lock cannot shorten the release cooldown. */
static void test_ac6_release_time_follows_lock_wait(void)
{
	t_config config = get_config(4);
	t_sim sim;
	t_release_call call;
	long threshold;
	int lock_result;
	int attempts;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms();
	assert(sim.start_ms >= 0);
	set_taken(&sim.dongles[0]);
	set_taken(&sim.dongles[1]);
	assert(pthread_mutex_lock(&sim.dongles[1].mutex) == 0);
	call.coder = &sim.coders[0];
	call.result = -1;
	assert(pthread_create(&call.thread, NULL, call_release, &call) == 0);
	attempts = 0;
	while (1)
	{
		lock_result = pthread_mutex_trylock(&sim.dongles[0].mutex);
		if (lock_result == EBUSY)
			break ;
		assert(lock_result == 0);
		assert(pthread_mutex_unlock(&sim.dongles[0].mutex) == 0);
		assert(usleep(1000) == 0);
		assert(++attempts < 1000);
	}
	assert(usleep(30000) == 0);
	threshold = get_elapsed_ms(sim.start_ms);
	assert(threshold >= 0);
	assert(pthread_mutex_unlock(&sim.dongles[1].mutex) == 0);
	assert(pthread_join(call.thread, NULL) == 0);
	assert(call.result == 0);
	assert(sim.dongles[0].available == 1);
	assert(sim.dongles[1].available == 1);
	assert(sim.dongles[0].released_at >= threshold);
	assert(sim.dongles[1].released_at >= threshold);
	assert_unlocked(&sim.dongles[0]);
	assert_unlocked(&sim.dongles[1]);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	alarm(60);
	test_ac1_releases_pair_at_current_time();
	test_ac2_wakes_both_waiters();
	test_ac3_preserves_queue_and_cooldown();
	test_ac4_releases_single_dongle();
	test_ac5_rejects_invalid_input_unchanged();
	test_ac6_release_time_follows_lock_wait();
	alarm(0);
	return (0);
}
