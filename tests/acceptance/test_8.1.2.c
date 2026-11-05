#include "main.h"

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct s_fields
{
	int		id;
	int		first_dongle;
	int		second_dongle;
	long	last_compile_start;
	int		compile_count;
} t_fields;

typedef struct s_call
{
	t_sim			*sim;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int			started;
	int			done;
	int			result;
	int			met;
} t_call;

static t_config get_config(int coders, int target, t_mode mode)
{
	t_config config;

	config.number_of_coders = coders;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = target;
	config.dongle_cooldown = 50;
	config.mode = mode;
	return (config);
}

static t_fields get_fields(const t_coder *coder)
{
	t_fields fields;

	fields.id = coder->id;
	fields.first_dongle = coder->first_dongle;
	fields.second_dongle = coder->second_dongle;
	fields.last_compile_start = coder->last_compile_start;
	fields.compile_count = coder->compile_count;
	return (fields);
}

static void assert_fields(const t_coder *coder, t_fields fields)
{
	assert(coder->id == fields.id);
	assert(coder->first_dongle == fields.first_dongle);
	assert(coder->second_dongle == fields.second_dongle);
	assert(coder->last_compile_start == fields.last_compile_start);
	assert(coder->compile_count == fields.compile_count);
}

static void assert_met(t_sim *sim, int expected)
{
	t_fields before[4];
	long start_ms;
	int stopped;
	int met;
	int i;

	assert(sim->config.number_of_coders <= 4);
	start_ms = sim->start_ms;
	stopped = sim->stopped;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		before[i] = get_fields(&sim->coders[i]);
		i++;
	}
	met = 7;
	alarm(1);
	assert(get_target_met(sim, &met) == 0);
	alarm(0);
	assert(met == expected);
	assert(sim->start_ms == start_ms);
	assert(sim->stopped == stopped);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		assert_fields(&sim->coders[i], before[i]);
		assert(pthread_mutex_trylock(&sim->coders[i].mutex) == 0);
		assert(pthread_mutex_unlock(&sim->coders[i].mutex) == 0);
		i++;
	}
}

static void set_counts(t_sim *sim, const int *counts)
{
	int i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].compile_count = counts[i];
		i++;
	}
}

/* AC1: one coder below target reports false without mutation, locks or output. */
static void test_ac1_one_coder_below_target(void)
{
	t_config config = get_config(4, 3, MODE_FIFO);
	const int counts[4] = {3, 3, 2, 3};
	t_sim sim;
	int pipefd[2];
	int saved_stdout;
	char byte;

	assert(init_simulation(&sim, &config) == 0);
	set_counts(&sim, counts);
	assert(pipe(pipefd) == 0);
	saved_stdout = dup(STDOUT_FILENO);
	assert(saved_stdout >= 0);
	assert(dup2(pipefd[1], STDOUT_FILENO) == STDOUT_FILENO);
	assert_met(&sim, 0);
	assert(dup2(saved_stdout, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(saved_stdout) == 0);
	assert(close(pipefd[1]) == 0);
	alarm(1);
	assert(read(pipefd[0], &byte, 1) == 0);
	alarm(0);
	assert(close(pipefd[0]) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC2: scan coverage includes equality, excess, first and last shortfalls. */
static void test_ac2_scan_coverage(void)
{
	t_config config = get_config(4, 3, MODE_FIFO);
	const int above[4] = {3, 4, 3, INT_MAX};
	const int zeros[4] = {0, 0, 0, 0};
	const int first[4] = {2, 3, 3, 3};
	const int last[4] = {3, 3, 3, 2};
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	set_counts(&sim, above);
	assert_met(&sim, 1);
	set_counts(&sim, zeros);
	assert_met(&sim, 0);
	set_counts(&sim, first);
	assert_met(&sim, 0);
	set_counts(&sim, last);
	assert_met(&sim, 0);
	assert(free_sim(&sim) == 0);
}

/* AC3: INT_MAX and a synthetic single-coder state use their configured target. */
static void test_ac3_other_targets(void)
{
	t_config config = get_config(4, INT_MAX, MODE_FIFO);
	const int all_max[4] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};
	const int one_short[4] = {INT_MAX, INT_MAX, INT_MAX - 1, INT_MAX};
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	set_counts(&sim, all_max);
	assert_met(&sim, 1);
	set_counts(&sim, one_short);
	assert_met(&sim, 0);
	assert(free_sim(&sim) == 0);
	config = get_config(1, 2, MODE_EDF);
	assert(init_simulation(&sim, &config) == 0);
	sim.coders[0].compile_count = 1;
	assert_met(&sim, 0);
	sim.coders[0].compile_count = 2;
	assert_met(&sim, 1);
	assert(free_sim(&sim) == 0);
}

/* AC4: zero required is met for both four-coder FIFO and one-coder EDF. */
static void test_ac4_zero_required(void)
{
	t_config config = get_config(4, 0, MODE_FIFO);
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	assert_met(&sim, 1);
	assert(free_sim(&sim) == 0);
	config = get_config(1, 0, MODE_EDF);
	assert(init_simulation(&sim, &config) == 0);
	assert_met(&sim, 1);
	assert(free_sim(&sim) == 0);
}

static void *get_after_handshake(void *arg)
{
	t_call *call = (t_call *)arg;

	assert(pthread_mutex_lock(&call->mutex) == 0);
	call->started = 1;
	assert(pthread_cond_signal(&call->cond) == 0);
	assert(pthread_mutex_unlock(&call->mutex) == 0);
	call->result = get_target_met(call->sim, &call->met);
	assert(pthread_mutex_lock(&call->mutex) == 0);
	call->done = 1;
	assert(pthread_mutex_unlock(&call->mutex) == 0);
	return (NULL);
}

static void wait_started(t_call *call)
{
	struct timespec limit;

	assert(clock_gettime(CLOCK_REALTIME, &limit) == 0);
	limit.tv_sec += 2;
	assert(pthread_mutex_lock(&call->mutex) == 0);
	while (!call->started)
		assert(pthread_cond_timedwait(&call->cond, &call->mutex, &limit) == 0);
	assert(pthread_mutex_unlock(&call->mutex) == 0);
}

/* AC5: a held coder mutex delays the query until its count is updated. */
static void test_ac5_contention_reads_updated_count(void)
{
	t_config config = get_config(4, 3, MODE_FIFO);
	const int counts[4] = {3, 3, 2, 3};
	t_sim sim;
	t_call call;
	pthread_t thread;

	assert(init_simulation(&sim, &config) == 0);
	set_counts(&sim, counts);
	memset(&call, 0, sizeof(call));
	call.sim = &sim;
	call.met = 7;
	assert(pthread_mutex_init(&call.mutex, NULL) == 0);
	assert(pthread_cond_init(&call.cond, NULL) == 0);
	assert(pthread_mutex_lock(&sim.coders[2].mutex) == 0);
	alarm(2);
	assert(pthread_create(&thread, NULL, get_after_handshake, &call) == 0);
	wait_started(&call);
	usleep(50000);
	assert(pthread_mutex_lock(&call.mutex) == 0);
	assert(call.done == 0);
	assert(pthread_mutex_unlock(&call.mutex) == 0);
	sim.coders[2].compile_count = 3;
	assert(pthread_mutex_unlock(&sim.coders[2].mutex) == 0);
	assert(pthread_join(thread, NULL) == 0);
	alarm(0);
	assert(call.result == 0);
	assert(call.met == 1);
	assert(pthread_cond_destroy(&call.cond) == 0);
	assert(pthread_mutex_destroy(&call.mutex) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC6: invalid inputs are rejected before changing the output or counts. */
static void test_ac6_reject_invalid_input(void)
{
	t_config config = get_config(4, 3, MODE_FIFO);
	const int counts[4] = {3, 3, 2, 3};
	t_sim sim;
	t_coder *saved_coders;
	int met;
	int i;

	assert(init_simulation(&sim, &config) == 0);
	set_counts(&sim, counts);
	met = 7;
	assert(get_target_met(NULL, &met) == -1 && met == 7);
	assert(get_target_met(&sim, NULL) == -1);
	saved_coders = sim.coders;
	sim.coders = NULL;
	assert(get_target_met(&sim, &met) == -1 && met == 7);
	sim.config.number_of_compiles_required = 0;
	assert(get_target_met(&sim, &met) == -1 && met == 7);
	sim.coders = saved_coders;
	sim.config.number_of_coders = 0;
	assert(get_target_met(&sim, &met) == -1 && met == 7);
	sim.config.number_of_compiles_required = 3;
	assert(get_target_met(&sim, &met) == -1 && met == 7);
	sim.config.number_of_coders = -1;
	assert(get_target_met(&sim, &met) == -1 && met == 7);
	sim.config.number_of_coders = 4;
	i = 0;
	while (i < 4)
	{
		assert(sim.coders[i].compile_count == counts[i]);
		i++;
	}
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	test_ac1_one_coder_below_target();
	test_ac2_scan_coverage();
	test_ac3_other_targets();
	test_ac4_zero_required();
	test_ac5_contention_reads_updated_count();
	test_ac6_reject_invalid_input();
	return (0);
}
