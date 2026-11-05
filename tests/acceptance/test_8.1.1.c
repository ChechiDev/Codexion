#include "main.h"

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct s_fields
{
	int	id;
	int	first_dongle;
	int	second_dongle;
	long	last_compile_start;
	int	compile_count;
} t_fields;

typedef struct s_call
{
	t_coder			*coder;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				started;
	int				done;
	int				result;
	int				burned;
} t_call;

static t_config get_config(int burnout)
{
	t_config config;

	config.number_of_coders = 4;
	config.time_to_burnout = burnout;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = MODE_FIFO;
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

static void assert_unlocked(t_coder *coder)
{
	assert(pthread_mutex_trylock(&coder->mutex) == 0);
	assert(pthread_mutex_unlock(&coder->mutex) == 0);
}

static void assert_result(t_coder *coder, long start, int expected)
{
	int burned;

	coder->last_compile_start = start;
	burned = 7;
	assert(get_burned_out(coder, &burned) == 0);
	assert(burned == expected);
}

/* AC1: deadline boundary, no state changes, unlocked mutex, no stdout. */
static void test_ac1_deadline_boundary_and_no_side_effects(void)
{
	t_config config = get_config(800);
	t_sim sim;
	t_fields before[4];
	const long starts[4] = {0, -799, -800, -801};
	const int expected[4] = {0, 0, 1, 1};
	int pipefd[2];
	int saved_stdout;
	char byte;
	int i;
	int j;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = LONG_MAX;
	assert(pipe(pipefd) == 0);
	saved_stdout = dup(STDOUT_FILENO);
	assert(saved_stdout >= 0);
	assert(dup2(pipefd[1], STDOUT_FILENO) == STDOUT_FILENO);
	i = 0;
	while (i < 4)
	{
		sim.coders[2].last_compile_start = starts[i];
		j = 0;
		while (j < 4)
		{
			before[j] = get_fields(&sim.coders[j]);
			j++;
		}
		assert_result(&sim.coders[2], starts[i], expected[i]);
		j = 0;
		while (j < 4)
		{
			assert_fields(&sim.coders[j], before[j]);
			j++;
		}
		assert(sim.start_ms == LONG_MAX);
		assert(sim.stopped == 0);
		assert_unlocked(&sim.coders[2]);
		i++;
	}
	assert(dup2(saved_stdout, STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(saved_stdout) == 0);
	assert(close(pipefd[1]) == 0);
	alarm(1);
	assert(read(pipefd[0], &byte, 1) == 0);
	alarm(0);
	assert(close(pipefd[0]) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC2: a 300 ms duration uses its own deadline. */
static void test_ac2_other_duration(void)
{
	t_config config = get_config(300);
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = LONG_MAX;
	assert_result(&sim.coders[2], -299, 0);
	assert_result(&sim.coders[2], -300, 1);
	assert(free_sim(&sim) == 0);
}

/* AC3: a real elapsed clock beyond the deadline reports burnout. */
static void test_ac3_real_clock(void)
{
	t_config config = get_config(800);
	t_sim sim;
	long now;

	assert(init_simulation(&sim, &config) == 0);
	now = get_time_ms();
	assert(now >= 1000);
	sim.start_ms = now - 1000;
	assert_result(&sim.coders[0], 0, 1);
	assert(free_sim(&sim) == 0);
}

/* AC4: meeting the individual target does not exempt a coder. */
static void test_ac4_satisfied_coder_still_burns(void)
{
	t_config config = get_config(800);
	t_sim sim;
	int i;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = LONG_MAX;
	sim.coders[1].compile_count = 3;
	assert_result(&sim.coders[1], -800, 1);
	i = 0;
	while (i < 4)
	{
		if (i != 1)
			assert(sim.coders[i].compile_count == 0);
		i++;
	}
	assert(free_sim(&sim) == 0);
}

/* AC5: extreme compile starts cannot corrupt the result. */
static void test_ac5_extreme_starts(void)
{
	t_config config = get_config(800);
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = LONG_MAX;
	assert_result(&sim.coders[2], LONG_MIN, 1);
	assert_result(&sim.coders[2], LONG_MAX, 0);
	assert(free_sim(&sim) == 0);
}

static void *get_after_handshake(void *arg)
{
	t_call *call = (t_call *)arg;

	assert(pthread_mutex_lock(&call->mutex) == 0);
	call->started = 1;
	assert(pthread_cond_signal(&call->cond) == 0);
	assert(pthread_mutex_unlock(&call->mutex) == 0);
	call->result = get_burned_out(call->coder, &call->burned);
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

/* AC6: a held coder mutex delays the read until after renewal. */
static void test_ac6_contention_reads_renewed_start(void)
{
	t_config config = get_config(800);
	t_sim sim;
	t_call call;
	pthread_t thread;

	assert(init_simulation(&sim, &config) == 0);
	sim.start_ms = LONG_MAX;
	sim.coders[1].last_compile_start = -800;
	memset(&call, 0, sizeof(call));
	call.coder = &sim.coders[1];
	call.burned = 7;
	assert(pthread_mutex_init(&call.mutex, NULL) == 0);
	assert(pthread_cond_init(&call.cond, NULL) == 0);
	assert(pthread_mutex_lock(&sim.coders[1].mutex) == 0);
	assert(pthread_create(&thread, NULL, get_after_handshake, &call) == 0);
	wait_started(&call);
	usleep(50000);
	assert(pthread_mutex_lock(&call.mutex) == 0);
	assert(call.done == 0);
	assert(pthread_mutex_unlock(&call.mutex) == 0);
	sim.coders[1].last_compile_start = 0;
	assert(pthread_mutex_unlock(&sim.coders[1].mutex) == 0);
	alarm(2);
	assert(pthread_join(thread, NULL) == 0);
	alarm(0);
	assert(call.result == 0);
	assert(call.burned == 0);
	assert(pthread_cond_destroy(&call.cond) == 0);
	assert(pthread_mutex_destroy(&call.mutex) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC7: null inputs fail without changing output or initialized coder fields. */
static void test_ac7_reject_null_inputs(void)
{
	t_config config = get_config(800);
	t_sim sim;
	t_fields before;
	t_sim *saved_sim;
	int burned;

	assert(init_simulation(&sim, &config) == 0);
	before = get_fields(&sim.coders[1]);
	burned = 7;
	assert(get_burned_out(NULL, &burned) == -1);
	assert(burned == 7);
	saved_sim = sim.coders[1].sim;
	sim.coders[1].sim = NULL;
	assert(get_burned_out(&sim.coders[1], &burned) == -1);
	assert(burned == 7);
	assert_fields(&sim.coders[1], before);
	sim.coders[1].sim = saved_sim;
	assert(get_burned_out(&sim.coders[1], NULL) == -1);
	assert_fields(&sim.coders[1], before);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	test_ac1_deadline_boundary_and_no_side_effects();
	test_ac2_other_duration();
	test_ac3_real_clock();
	test_ac4_satisfied_coder_still_burns();
	test_ac5_extreme_starts();
	test_ac6_contention_reads_renewed_start();
	test_ac7_reject_null_inputs();
	return (0);
}
