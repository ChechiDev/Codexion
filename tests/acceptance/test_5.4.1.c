#include "main.h"

#include <assert.h>
#include <string.h>

static void init_test_sim(t_sim *sim, int n, int with_dongles,
		int with_coders)
{
	memset(sim, 0, sizeof(*sim));
	sim->config.number_of_coders = n;
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->ticket_mutex, NULL) == 0);
	if (with_dongles)
		assert(init_dongles(&sim->dongles, n) == 0);
	if (with_coders)
		assert(init_coders(&sim->coders, sim, n) == 0);
}

/* AC1: a full, manually assembled three-coder simulation is freed. */
static void test_ac1_free_full_sim(void)
{
	t_sim sim;

	init_test_sim(&sim, 3, 1, 1);
	assert(free_sim(&sim) == 0);
	assert(sim.dongles == NULL);
	assert(sim.coders == NULL);
}

/* AC2: a dongle-only simulation and one with no arrays are freed. */
static void test_ac2_free_partial_arrays(void)
{
	t_sim sim;

	init_test_sim(&sim, 3, 1, 0);
	assert(sim.dongles != NULL);
	assert(sim.coders == NULL);
	assert(free_sim(&sim) == 0);
	assert(sim.dongles == NULL);
	assert(sim.coders == NULL);
	init_test_sim(&sim, 3, 0, 0);
	assert(free_sim(&sim) == 0);
	assert(sim.dongles == NULL);
	assert(sim.coders == NULL);
}

/* AC3: used resources are freed without changing retained sim state. */
static void test_ac3_preserve_state_after_use(void)
{
	t_sim sim;
	t_request request;
	int i;

	init_test_sim(&sim, 3, 1, 1);
	sim.config.time_to_burnout = 101;
	sim.config.time_to_compile = 23;
	sim.config.time_to_debug = 17;
	sim.config.time_to_refactor = 19;
	sim.config.number_of_compiles_required = 5;
	sim.config.dongle_cooldown = 11;
	sim.config.mode = MODE_EDF;
	sim.start_ms = 1000;
	sim.stopped = 1;
	sim.next_ticket = 7;
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_lock(&sim.ticket_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.ticket_mutex) == 0);
	i = 0;
	while (i < 3)
	{
		assert(pthread_mutex_lock(&sim.dongles[i].mutex) == 0);
		assert(pthread_mutex_unlock(&sim.dongles[i].mutex) == 0);
		assert(pthread_mutex_lock(&sim.coders[i].mutex) == 0);
		assert(pthread_mutex_unlock(&sim.coders[i].mutex) == 0);
		i++;
	}
	request.key = 1;
	request.ticket = 7;
	request.coder = 1;
	assert(push_request(&sim.dongles[0].heap, &request) == 0);
	sim.coders[0].compile_count = 2;
	assert(free_sim(&sim) == 0);
	assert(sim.dongles == NULL && sim.coders == NULL);
	assert(sim.config.number_of_coders == 3);
	assert(sim.config.time_to_burnout == 101);
	assert(sim.config.time_to_compile == 23);
	assert(sim.config.time_to_debug == 17);
	assert(sim.config.time_to_refactor == 19);
	assert(sim.config.number_of_compiles_required == 5);
	assert(sim.config.dongle_cooldown == 11);
	assert(sim.config.mode == MODE_EDF);
	assert(sim.start_ms == 1000);
	assert(sim.stopped == 1);
	assert(sim.next_ticket == 7);
}

/* AC4: the same struct can be rebuilt after teardown and freed again. */
static void test_ac4_rebuild_same_struct(void)
{
	t_sim sim;
	int i;

	i = 0;
	while (i < 2)
	{
		init_test_sim(&sim, 3, 1, 1);
		assert(free_sim(&sim) == 0);
		assert(sim.dongles == NULL);
		assert(sim.coders == NULL);
		i++;
	}
}

/* AC5: one coder with its single dongle is freed. */
static void test_ac5_free_one_coder_sim(void)
{
	t_sim sim;

	init_test_sim(&sim, 1, 1, 1);
	assert(free_sim(&sim) == 0);
	assert(sim.dongles == NULL);
	assert(sim.coders == NULL);
}

/* AC6: NULL is rejected without dereferencing it. */
static void test_ac6_reject_null(void)
{
	assert(free_sim(NULL) == -1);
}

int main(void)
{
	test_ac1_free_full_sim();
	test_ac2_free_partial_arrays();
	test_ac3_preserve_state_after_use();
	test_ac4_rebuild_same_struct();
	test_ac5_free_one_coder_sim();
	test_ac6_reject_null();
	return (0);
}
