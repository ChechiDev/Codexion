#include "main.h"

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct s_count_work
{
	t_coder	*coder;
	int		result;
} t_count_work;

typedef struct s_blocked_call
{
	t_coder			*coder;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				started;
	int				done;
	int				result;
} t_blocked_call;

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

/* AC1: three completed compiles change only the selected coder's count. */
static void test_ac1_increment_selected_coder(void)
{
	t_config config = get_config();
	t_sim sim;
	int i;

	assert(init_simulation(&sim, &config) == 0);
	sim.coders[1].last_compile_start = -12345;
	i = 1;
	while (i <= 3)
	{
		assert(set_compile_count(&sim.coders[1]) == 0);
		assert(sim.coders[1].compile_count == i);
		i++;
	}
	assert(sim.coders[1].id == 2);
	assert(sim.coders[1].first_dongle == 1);
	assert(sim.coders[1].second_dongle == 2);
	assert(sim.coders[1].last_compile_start == -12345);
	assert_unlocked(&sim.coders[1].mutex);
	i = 0;
	while (i < 4)
	{
		if (i != 1)
			assert(sim.coders[i].compile_count == 0);
		i++;
	}
	assert(free_sim(&sim) == 0);
}

/* AC2: the maximum is reachable but cannot overflow; negatives are rejected. */
static void test_ac2_reject_invalid_counts(void)
{
	t_config config = get_config();
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	sim.coders[1].compile_count = INT_MAX - 1;
	assert(set_compile_count(&sim.coders[1]) == 0);
	assert(sim.coders[1].compile_count == INT_MAX);
	assert_unlocked(&sim.coders[1].mutex);
	assert(set_compile_count(&sim.coders[1]) == -1);
	assert(sim.coders[1].compile_count == INT_MAX);
	assert_unlocked(&sim.coders[1].mutex);
	sim.coders[1].compile_count = -1;
	assert(set_compile_count(&sim.coders[1]) == -1);
	assert(sim.coders[1].compile_count == -1);
	assert_unlocked(&sim.coders[1].mutex);
	assert(free_sim(&sim) == 0);
}

static void *count_many(void *arg)
{
	t_count_work *work = (t_count_work *)arg;
	int i;

	work->result = 0;
	i = 0;
	while (i < 1000)
	{
		if (set_compile_count(work->coder) != 0)
			work->result = -1;
		i++;
	}
	return (NULL);
}

/* AC3: eight concurrent callers each contribute exactly 1000 increments. */
static void test_ac3_concurrent_increments(void)
{
	t_config config = get_config();
	t_sim sim;
	pthread_t threads[8];
	t_count_work work[8];
	int i;

	assert(init_simulation(&sim, &config) == 0);
	i = 0;
	while (i < 8)
	{
		work[i].coder = &sim.coders[1];
		assert(pthread_create(&threads[i], NULL, count_many, &work[i]) == 0);
		i++;
	}
	i = 0;
	while (i < 8)
	{
		assert(pthread_join(threads[i], NULL) == 0);
		assert(work[i].result == 0);
		i++;
	}
	assert(sim.coders[1].compile_count == 8000);
	assert(free_sim(&sim) == 0);
}

static void *count_after_handshake(void *arg)
{
	t_blocked_call *call = (t_blocked_call *)arg;

	assert(pthread_mutex_lock(&call->mutex) == 0);
	call->started = 1;
	assert(pthread_cond_signal(&call->cond) == 0);
	assert(pthread_mutex_unlock(&call->mutex) == 0);
	call->result = set_compile_count(call->coder);
	assert(pthread_mutex_lock(&call->mutex) == 0);
	call->done = 1;
	assert(pthread_mutex_unlock(&call->mutex) == 0);
	return (NULL);
}

static void wait_started(t_blocked_call *call)
{
	struct timespec limit;

	assert(clock_gettime(CLOCK_REALTIME, &limit) == 0);
	limit.tv_sec += 1;
	assert(pthread_mutex_lock(&call->mutex) == 0);
	while (!call->started)
		assert(pthread_cond_timedwait(&call->cond, &call->mutex, &limit) == 0);
	assert(pthread_mutex_unlock(&call->mutex) == 0);
}

/* AC4: the call stays incomplete while the selected coder mutex is held. */
static void test_ac4_wait_for_coder_mutex(void)
{
	t_config config = get_config();
	t_sim sim;
	t_blocked_call call;
	pthread_t thread;
	long start;
	long now;

	assert(init_simulation(&sim, &config) == 0);
	memset(&call, 0, sizeof(call));
	call.coder = &sim.coders[1];
	assert(pthread_mutex_init(&call.mutex, NULL) == 0);
	assert(pthread_cond_init(&call.cond, NULL) == 0);
	assert(pthread_mutex_lock(&sim.coders[1].mutex) == 0);
	assert(pthread_create(&thread, NULL, count_after_handshake, &call) == 0);
	wait_started(&call);
	start = get_time_ms();
	assert(start >= 0);
	now = start;
	while (now - start < 50)
	{
		usleep(1000);
		now = get_time_ms();
		assert(now >= start);
	}
	assert(pthread_mutex_lock(&call.mutex) == 0);
	assert(call.done == 0);
	assert(pthread_mutex_unlock(&call.mutex) == 0);
	assert(sim.coders[1].compile_count == 0);
	assert(pthread_mutex_unlock(&sim.coders[1].mutex) == 0);
	alarm(1);
	assert(pthread_join(thread, NULL) == 0);
	alarm(0);
	assert(call.result == 0);
	assert(sim.coders[1].compile_count == 1);
	assert(pthread_cond_destroy(&call.cond) == 0);
	assert(pthread_mutex_destroy(&call.mutex) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC5: a null coder is rejected without dereferencing it. */
static void test_ac5_reject_null_coder(void)
{
	assert(set_compile_count(NULL) == -1);
}

int main(void)
{
	test_ac1_increment_selected_coder();
	test_ac2_reject_invalid_counts();
	test_ac3_concurrent_increments();
	test_ac4_wait_for_coder_mutex();
	test_ac5_reject_null_coder();
	return (0);
}
