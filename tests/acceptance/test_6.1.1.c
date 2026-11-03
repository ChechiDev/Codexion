#include "main.h"

#include <assert.h>
#include <limits.h>
#include <pthread.h>

#define THREAD_COUNT 8
#define TICKETS_PER_THREAD 1000
#define TOTAL_TICKETS (THREAD_COUNT * TICKETS_PER_THREAD)

typedef struct s_ticket_worker
{
	t_sim	*sim;
	long	tickets[TICKETS_PER_THREAD];
	int		failed;
}	t_ticket_worker;

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

static void assert_ticket_mutex_unlocked(t_sim *sim)
{
	assert(pthread_mutex_trylock(&sim->ticket_mutex) == 0);
	assert(pthread_mutex_unlock(&sim->ticket_mutex) == 0);
}

/* AC1: three calls hand out 0, 1, 2 and leave the mutex unlocked. */
static void test_ac1_sequential_tickets(void)
{
	t_config config = get_config();
	t_sim sim;
	long out = -1;
	long expected;

	assert(init_simulation(&sim, &config) == 0);
	expected = 0;
	while (expected < 3)
	{
		assert(take_ticket(&sim, &out) == 0);
		assert(out == expected);
		expected++;
	}
	assert(sim.next_ticket == 3);
	assert_ticket_mutex_unlocked(&sim);
	assert(free_sim(&sim) == 0);
}

/* AC2: caller-set counters 41 and LONG_MAX - 1 are issued once. */
static void test_ac2_counter_boundaries(void)
{
	t_config config = get_config();
	t_sim sim;
	long out = -1;

	assert(init_simulation(&sim, &config) == 0);
	sim.next_ticket = 41;
	assert(take_ticket(&sim, &out) == 0);
	assert(out == 41);
	assert(sim.next_ticket == 42);
	sim.next_ticket = LONG_MAX - 1;
	assert(take_ticket(&sim, &out) == 0);
	assert(out == LONG_MAX - 1);
	assert(sim.next_ticket == LONG_MAX);
	assert(free_sim(&sim) == 0);
}

/* AC3: exhausted and negative counters fail without changing either value. */
static void test_ac3_reject_exhausted_or_negative(void)
{
	t_config config = get_config();
	t_sim sim;
	long out = -17;

	assert(init_simulation(&sim, &config) == 0);
	sim.next_ticket = LONG_MAX;
	assert(take_ticket(&sim, &out) == -1);
	assert(out == -17);
	assert(sim.next_ticket == LONG_MAX);
	assert_ticket_mutex_unlocked(&sim);
	sim.next_ticket = -1;
	assert(take_ticket(&sim, &out) == -1);
	assert(out == -17);
	assert(sim.next_ticket == -1);
	assert_ticket_mutex_unlocked(&sim);
	assert(free_sim(&sim) == 0);
}

static void *collect_tickets(void *arg)
{
	t_ticket_worker *worker = (t_ticket_worker *)arg;
	int i;

	i = 0;
	while (i < TICKETS_PER_THREAD)
	{
		if (take_ticket(worker->sim, &worker->tickets[i]) != 0)
		{
			worker->failed = 1;
			return (NULL);
		}
		i++;
	}
	return (NULL);
}

/* AC4: concurrent callers get each ticket 0..7999 once, ordered per caller. */
static void test_ac4_concurrent_unique_tickets(void)
{
	t_config config = get_config();
	t_sim sim;
	t_ticket_worker workers[THREAD_COUNT];
	pthread_t threads[THREAD_COUNT];
	int seen[TOTAL_TICKETS] = {0};
	int i;
	int j;
	long ticket;

	assert(init_simulation(&sim, &config) == 0);
	i = 0;
	while (i < THREAD_COUNT)
	{
		workers[i].sim = &sim;
		workers[i].failed = 0;
		assert(pthread_create(&threads[i], NULL, collect_tickets,
				&workers[i]) == 0);
		i++;
	}
	i = 0;
	while (i < THREAD_COUNT)
	{
		assert(pthread_join(threads[i], NULL) == 0);
		assert(workers[i].failed == 0);
		j = 0;
		while (j < TICKETS_PER_THREAD)
		{
			ticket = workers[i].tickets[j];
			assert(ticket >= 0 && ticket < TOTAL_TICKETS);
			assert(seen[ticket] == 0);
			seen[ticket] = 1;
			if (j > 0)
				assert(workers[i].tickets[j - 1] < ticket);
			j++;
		}
		i++;
	}
	i = 0;
	while (i < TOTAL_TICKETS)
		assert(seen[i++] == 1);
	assert(sim.next_ticket == TOTAL_TICKETS);
	assert(free_sim(&sim) == 0);
}

/* AC5: null inputs fail without changing the output or ticket counter. */
static void test_ac5_reject_null_inputs(void)
{
	t_config config = get_config();
	t_sim sim;
	long out = -17;

	assert(take_ticket(NULL, &out) == -1);
	assert(out == -17);
	assert(init_simulation(&sim, &config) == 0);
	assert(take_ticket(&sim, NULL) == -1);
	assert(sim.next_ticket == 0);
	assert_ticket_mutex_unlocked(&sim);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	test_ac1_sequential_tickets();
	test_ac2_counter_boundaries();
	test_ac3_reject_exhausted_or_negative();
	test_ac4_concurrent_unique_tickets();
	test_ac5_reject_null_inputs();
	return (0);
}
