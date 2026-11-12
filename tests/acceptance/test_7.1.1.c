#include "main.h"

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct s_call
{
	t_coder			*coder;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				started;
	int				done;
	int				result;
} t_call;

static t_config get_config(void)
{
	t_config config;

	config.number_of_coders = 4;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
	return (config);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

static void set_sentinel(t_coder *coder, long value)
{
	assert(pthread_mutex_lock(&coder->mutex) == 0);
	coder->last_compile_start = value;
	assert(pthread_mutex_unlock(&coder->mutex) == 0);
}

/* AC1: a successful call records elapsed time and changes only its own start. */
static void test_ac1_record_elapsed_start(void)
{
	t_config config = get_config();
	t_sim sim;
	long start;
	long before;
	long after;
	int i;

	assert(init_simulation(&sim, &config) == 0);
	start = get_time_ms() - 500;
	assert(start > 0);
	sim.start_ms = start;
	sim.coders[1].compile_count = 7;
	set_sentinel(&sim.coders[1], 100);
	before = get_elapsed_ms(start);
	assert(before >= 0);
	assert(set_compile_start(&sim.coders[1]) == 0);
	after = get_elapsed_ms(start);
	assert(after >= before);
	assert(sim.coders[1].last_compile_start >= before);
	assert(sim.coders[1].last_compile_start <= after);
	assert(sim.coders[1].id == 2);
	assert(sim.coders[1].first_dongle == 1);
	assert(sim.coders[1].second_dongle == 2);
	assert(sim.coders[1].compile_count == 7);
	assert(sim.coders[1].sim == &sim);
	assert(sim.start_ms == start);
	assert_unlocked(&sim.coders[1].mutex);
	i = 0;
	while (i < 4)
	{
		if (i != 1)
			assert(sim.coders[i].last_compile_start == 0);
		i++;
	}
	assert(free_sim(&sim) == 0);
}

/* AC2: a later call overwrites the timestamp with a later start. */
static void test_ac2_overwrite_start(void)
{
	t_config config = get_config();
	t_sim sim;
	long first;
	long now;
	long wait_from;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 500;
	assert(sim.start_ms > 0);
	assert(set_compile_start(&sim.coders[1]) == 0);
	first = sim.coders[1].last_compile_start;
	wait_from = get_time_ms();
	assert(wait_from >= 0);
	now = wait_from;
	while (now - wait_from < 20)
	{
		usleep(1000);
		now = get_time_ms();
		assert(now >= wait_from);
	}
	assert(set_compile_start(&sim.coders[1]) == 0);
	assert(sim.coders[1].last_compile_start >= first + 20);
	assert(free_sim(&sim) == 0);
}

/* AC3: a future simulation start clamps the recorded value to zero. */
static void test_ac3_clamp_future_start(void)
{
	t_config config = get_config();
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = LONG_MAX;
	set_sentinel(&sim.coders[1], 100);
	assert(set_compile_start(&sim.coders[1]) == 0);
	assert(sim.coders[1].last_compile_start == 0);
	assert(free_sim(&sim) == 0);
}

static void *call_set_compile_start(void *arg)
{
	t_call *call = (t_call *)arg;

	assert(pthread_mutex_lock(&call->mutex) == 0);
	call->started = 1;
	assert(pthread_cond_signal(&call->cond) == 0);
	assert(pthread_mutex_unlock(&call->mutex) == 0);
	call->result = set_compile_start(call->coder);
	assert(pthread_mutex_lock(&call->mutex) == 0);
	call->done = 1;
	assert(pthread_mutex_unlock(&call->mutex) == 0);
	return (NULL);
}

static void wait_started(t_call *call)
{
	struct timespec limit;

	assert(clock_gettime(CLOCK_REALTIME, &limit) == 0);
	limit.tv_sec += 1;
	assert(pthread_mutex_lock(&call->mutex) == 0);
	while (!call->started)
		assert(pthread_cond_timedwait(&call->cond, &call->mutex, &limit) == 0);
	assert(pthread_mutex_unlock(&call->mutex) == 0);
}

/* AC4: a held coder mutex blocks the update, which uses post-unlock time. */
static void test_ac4_wait_for_coder_mutex(void)
{
	t_config config = get_config();
	t_sim sim;
	t_call call;
	pthread_t thread;
	long t0;
	long t1;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = get_time_ms() - 500;
	assert(sim.start_ms > 0);
	set_sentinel(&sim.coders[1], 100);
	memset(&call, 0, sizeof(call));
	call.coder = &sim.coders[1];
	assert(pthread_mutex_init(&call.mutex, NULL) == 0);
	assert(pthread_cond_init(&call.cond, NULL) == 0);
	assert(pthread_mutex_lock(&sim.coders[1].mutex) == 0);
	assert(pthread_create(&thread, NULL, call_set_compile_start, &call) == 0);
	wait_started(&call);
	usleep(50000);
	assert(pthread_mutex_lock(&call.mutex) == 0);
	assert(call.done == 0);
	assert(pthread_mutex_unlock(&call.mutex) == 0);
	assert(sim.coders[1].last_compile_start == 100);
	t0 = get_elapsed_ms(sim.start_ms);
	assert(t0 >= 0);
	assert(pthread_mutex_unlock(&sim.coders[1].mutex) == 0);
	alarm(1);
	assert(pthread_join(thread, NULL) == 0);
	alarm(0);
	t1 = get_elapsed_ms(sim.start_ms);
	assert(t1 >= t0);
	assert(call.result == 0);
	assert(sim.coders[1].last_compile_start >= t0);
	assert(sim.coders[1].last_compile_start <= t1);
	assert(pthread_cond_destroy(&call.cond) == 0);
	assert(pthread_mutex_destroy(&call.mutex) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC5: null coder or simulation returns an error without changing the start. */
static void test_ac5_reject_null_inputs(void)
{
	t_coder coder;

	assert(set_compile_start(NULL) == -1);
	memset(&coder, 0, sizeof(coder));
	coder.sim = NULL;
	coder.last_compile_start = -12345;
	assert(set_compile_start(&coder) == -1);
	assert(coder.last_compile_start == -12345);
}

int main(void)
{
	test_ac1_record_elapsed_start();
	test_ac2_overwrite_start();
	test_ac3_clamp_future_start();
	test_ac4_wait_for_coder_mutex();
	test_ac5_reject_null_inputs();
	return (0);
}
