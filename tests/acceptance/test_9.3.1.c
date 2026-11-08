#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_gate
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int ready;
	int departed;
	int open;
	int entered;
	int returned;
	int result;
} t_gate;

typedef struct s_join_call
{
	t_sim *sim;
	t_gate *gate;
	int coders;
} t_join_call;

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

static void init_gate(t_gate *gate)
{
	gate->ready = 0;
	gate->departed = 0;
	gate->open = 0;
	gate->entered = 0;
	gate->returned = 0;
	gate->result = -99;
	assert(pthread_mutex_init(&gate->mutex, NULL) == 0);
	assert(pthread_cond_init(&gate->cond, NULL) == 0);
}

static void destroy_gate(t_gate *gate)
{
	assert(pthread_cond_destroy(&gate->cond) == 0);
	assert(pthread_mutex_destroy(&gate->mutex) == 0);
}

static void *run_held_worker(void *arg)
{
	t_gate *gate;

	gate = arg;
	assert(pthread_mutex_lock(&gate->mutex) == 0);
	gate->ready++;
	assert(pthread_cond_broadcast(&gate->cond) == 0);
	while (!gate->open)
		assert(pthread_cond_wait(&gate->cond, &gate->mutex) == 0);
	gate->departed++;
	assert(pthread_cond_broadcast(&gate->cond) == 0);
	assert(pthread_mutex_unlock(&gate->mutex) == 0);
	return (NULL);
}

static void start_workers(t_sim *sim, t_gate *gate, int coders)
{
	int i;

	i = 0;
	while (i < coders)
	{
		assert(pthread_create(&sim->coders[i].thread, NULL,
				run_held_worker, gate) == 0);
		i++;
	}
	assert(pthread_create(&sim->monitor, NULL, run_held_worker, gate) == 0);
	assert(pthread_mutex_lock(&gate->mutex) == 0);
	while (gate->ready != coders + 1)
		assert(pthread_cond_wait(&gate->cond, &gate->mutex) == 0);
	assert(pthread_mutex_unlock(&gate->mutex) == 0);
}

static void open_gate(t_gate *gate)
{
	assert(pthread_mutex_lock(&gate->mutex) == 0);
	gate->open = 1;
	assert(pthread_cond_broadcast(&gate->cond) == 0);
	assert(pthread_mutex_unlock(&gate->mutex) == 0);
}

static void *run_join_call(void *arg)
{
	t_join_call *call;
	int result;

	call = arg;
	assert(pthread_mutex_lock(&call->gate->mutex) == 0);
	call->gate->entered = 1;
	assert(pthread_cond_broadcast(&call->gate->cond) == 0);
	assert(pthread_mutex_unlock(&call->gate->mutex) == 0);
	result = join_threads(call->sim, call->coders);
	assert(pthread_mutex_lock(&call->gate->mutex) == 0);
	call->gate->result = result;
	call->gate->returned = 1;
	assert(pthread_cond_broadcast(&call->gate->cond) == 0);
	assert(pthread_mutex_unlock(&call->gate->mutex) == 0);
	return (NULL);
}

static void assert_waits_and_joins(int coders)
{
	t_config config;
	t_sim sim;
	t_gate gate;
	t_join_call call;
	pthread_t caller;

	config = get_config();
	assert(init_simulation(&sim, &config) == 0);
	init_gate(&gate);
	start_workers(&sim, &gate, coders);
	call.sim = &sim;
	call.gate = &gate;
	call.coders = coders;
	assert(pthread_create(&caller, NULL, run_join_call, &call) == 0);
	assert(pthread_mutex_lock(&gate.mutex) == 0);
	while (!gate.entered)
		assert(pthread_cond_wait(&gate.cond, &gate.mutex) == 0);
	assert(pthread_mutex_unlock(&gate.mutex) == 0);
	assert(usleep(100000) == 0);
	assert(pthread_mutex_lock(&gate.mutex) == 0);
	assert(gate.returned == 0);
	assert(pthread_mutex_unlock(&gate.mutex) == 0);
	open_gate(&gate);
	assert(pthread_join(caller, NULL) == 0);
	assert(pthread_mutex_lock(&gate.mutex) == 0);
	assert(gate.returned == 1 && gate.result == 0);
	while (gate.departed != coders + 1)
		assert(pthread_cond_wait(&gate.cond, &gate.mutex) == 0);
	assert(pthread_mutex_unlock(&gate.mutex) == 0);
	destroy_gate(&gate);
	assert(free_sim(&sim) == 0);
}

/* AC1: the full join waits for four held coders and the held monitor. */
static void test_ac1_waits_for_every_thread(void)
{
	assert_waits_and_joins(4);
}

/* AC2: the partial join waits for two held coders and the held monitor. */
static void test_ac2_joins_only_started_coders(void)
{
	assert_waits_and_joins(2);
}

/* AC3: the zero-coder join waits for the held monitor. */
static void test_ac3_joins_monitor_only(void)
{
	assert_waits_and_joins(0);
}

/* AC4: NULL and out-of-range counts return promptly without joining. */
static void test_ac4_invalid_input_joins_nothing(void)
{
	t_config config;
	t_sim sim;
	t_gate gate;
	long before;
	long after;
	int counts[2];
	int case_index;
	int i;

	assert(join_threads(NULL, 0) == -1);
	config = get_config();
	counts[0] = -1;
	counts[1] = 5;
	case_index = 0;
	while (case_index < 2)
	{
		assert(init_simulation(&sim, &config) == 0);
		init_gate(&gate);
		start_workers(&sim, &gate, 4);
		before = get_time_ms();
		assert(before >= 0);
		assert(join_threads(&sim, counts[case_index]) == -1);
		after = get_time_ms();
		assert(after >= before && after - before <= 100);
		assert(pthread_mutex_lock(&gate.mutex) == 0);
		assert(gate.open == 0 && gate.ready == 5);
		assert(pthread_mutex_unlock(&gate.mutex) == 0);
		open_gate(&gate);
		i = 0;
		while (i < 4)
			assert(pthread_join(sim.coders[i++].thread, NULL) == 0);
		assert(pthread_join(sim.monitor, NULL) == 0);
		destroy_gate(&gate);
		assert(free_sim(&sim) == 0);
		case_index++;
	}
}

int main(void)
{
	if (getenv("CODEXION_UNDER_VALGRIND") != NULL)
		alarm(60);
	else
		alarm(20);
	test_ac1_waits_for_every_thread();
	test_ac2_joins_only_started_coders();
	test_ac3_joins_monitor_only();
	test_ac4_invalid_input_joins_nothing();
	alarm(0);
	return (0);
}
