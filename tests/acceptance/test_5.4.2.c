#include "main.h"

#include <assert.h>
#include <string.h>

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
	config.mode = MODE_EDF;
	return (config);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

static void assert_config(const t_config *actual, const t_config *expected)
{
	assert(actual->number_of_coders == expected->number_of_coders);
	assert(actual->time_to_burnout == expected->time_to_burnout);
	assert(actual->time_to_compile == expected->time_to_compile);
	assert(actual->time_to_debug == expected->time_to_debug);
	assert(actual->time_to_refactor == expected->time_to_refactor);
	assert(actual->number_of_compiles_required
		== expected->number_of_compiles_required);
	assert(actual->dongle_cooldown == expected->dongle_cooldown);
	assert(actual->mode == expected->mode);
}

static void assert_sim_state(t_sim *sim, const t_config *config)
{
	int i;
	int first;
	int second;

	assert_config(&sim->config, config);
	assert(sim->start_ms == 0);
	assert(sim->stopped == 0);
	assert(sim->next_ticket == 0);
	assert(sim->dongles != NULL);
	assert(sim->coders != NULL);
	assert_unlocked(&sim->stop_mutex);
	assert_unlocked(&sim->log_mutex);
	assert_unlocked(&sim->ticket_mutex);
	i = 0;
	while (i < config->number_of_coders)
	{
		assert(sim->dongles[i].available == 1);
		assert(sim->dongles[i].released_at == -1);
		assert(sim->dongles[i].heap.size == 0);
		assert(sim->dongles[i].heap.capacity == 2);
		assert_unlocked(&sim->dongles[i].mutex);
		assert(get_dongle_pair(i, config->number_of_coders,
				&first, &second) == 0);
		assert(sim->coders[i].id == i + 1);
		assert(sim->coders[i].first_dongle == first);
		assert(sim->coders[i].second_dongle == second);
		assert(sim->coders[i].last_compile_start == 0);
		assert(sim->coders[i].compile_count == 0);
		assert(sim->coders[i].sim == sim);
		assert_unlocked(&sim->coders[i].mutex);
		i++;
	}
}

/* AC1: a four-coder simulation has complete initial state and frees cleanly. */
static void test_ac1_initialize_four_coders(void)
{
	t_config config = get_config(4);
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	assert_sim_state(&sim, &config);
	assert(free_sim(&sim) == 0);
}

/* AC2: one and 200 coders have complete state and boundary seat mappings. */
static void test_ac2_initialize_boundary_counts(void)
{
	t_config config = get_config(1);
	t_sim sim;
	int i;

	assert(init_simulation(&sim, &config) == 0);
	assert_sim_state(&sim, &config);
	assert(sim.coders[0].first_dongle == 0);
	assert(sim.coders[0].second_dongle == 0);
	assert(free_sim(&sim) == 0);
	config.number_of_coders = 200;
	assert(init_simulation(&sim, &config) == 0);
	assert_sim_state(&sim, &config);
	i = 0;
	while (i < 200)
	{
		assert(sim.coders[i].first_dongle >= 0);
		assert(sim.coders[i].first_dongle < 200);
		assert(sim.coders[i].second_dongle >= 0);
		assert(sim.coders[i].second_dongle < 200);
		i++;
	}
	assert(sim.coders[199].first_dongle == 0);
	assert(sim.coders[199].second_dongle == 199);
	assert(free_sim(&sim) == 0);
}

/* AC3: the phase 03 stop-state API works on the initialized simulation. */
static void test_ac3_stop_state_api(void)
{
	t_config config = get_config(4);
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	assert(is_stopped(&sim) == 0);
	assert(set_stopped(&sim) == 0);
	assert(is_stopped(&sim) == 1);
	assert(free_sim(&sim) == 0);
}

/* AC4: initialization overwrites a nonzero, resource-free caller buffer. */
static void test_ac4_initialize_sentinel_buffer(void)
{
	t_config config = get_config(4);
	t_sim sim;

	memset(&sim, 0xa5, sizeof(sim));
	assert(init_simulation(&sim, &config) == 0);
	assert_sim_state(&sim, &config);
	assert(free_sim(&sim) == 0);
}

/* AC5: invalid inputs fail without changing the caller's bytes. */
static void test_ac5_reject_invalid_input_unchanged(void)
{
	t_config config = get_config(4);
	t_sim sim;
	t_sim before;

	memset(&sim, 0xa5, sizeof(sim));
	memcpy(&before, &sim, sizeof(sim));
	assert(init_simulation(&sim, NULL) == -1);
	assert(memcmp(&sim, &before, sizeof(sim)) == 0);
	config.number_of_coders = 0;
	assert(init_simulation(&sim, &config) == -1);
	assert(memcmp(&sim, &before, sizeof(sim)) == 0);
	config.number_of_coders = -1;
	assert(init_simulation(&sim, &config) == -1);
	assert(memcmp(&sim, &before, sizeof(sim)) == 0);
	config.number_of_coders = 4;
	assert(init_simulation(NULL, &config) == -1);
}

/* AC6: teardown allows the same struct to be initialized at another size. */
static void test_ac6_reinitialize_after_free(void)
{
	t_config config = get_config(3);
	t_sim sim;

	assert(init_simulation(&sim, &config) == 0);
	assert(free_sim(&sim) == 0);
	config.number_of_coders = 5;
	assert(init_simulation(&sim, &config) == 0);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	test_ac1_initialize_four_coders();
	test_ac2_initialize_boundary_counts();
	test_ac3_stop_state_api();
	test_ac4_initialize_sentinel_buffer();
	test_ac5_reject_invalid_input_unchanged();
	test_ac6_reinitialize_after_free();
	return (0);
}
