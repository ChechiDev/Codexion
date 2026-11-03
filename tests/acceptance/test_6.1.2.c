#include "main.h"

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#define WORKERS 4
#define REQUESTS_PER_WORKER 500
#define TOTAL_REQUESTS (WORKERS * REQUESTS_PER_WORKER)

typedef struct s_worker
{
	t_coder		*code_owner;
	t_request	requests[REQUESTS_PER_WORKER];
	int			failed;
}	t_worker;

static t_config get_config(t_mode mode)
{
	t_config config;

	config.number_of_coders = 4;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = mode;
	return (config);
}

static void assert_unlocked(pthread_mutex_t *mutex)
{
	assert(pthread_mutex_trylock(mutex) == 0);
	assert(pthread_mutex_unlock(mutex) == 0);
}

static void fill_sentinel(t_request *req)
{
	memset(req, 0xa5, sizeof(*req));
}

/* AC1: FIFO requests use consecutive tickets as keys and coder IDs. */
static void test_ac1_fifo_requests(void)
{
	t_config config = get_config(MODE_FIFO);
	t_sim sim;
	t_request req;

	assert(init_simulation(&sim, &config) == 0);
	assert(build_request(&sim.coders[0], &req) == 0);
	assert(req.ticket == 0 && req.key == 0 && req.coder == 1);
	assert(build_request(&sim.coders[2], &req) == 0);
	assert(req.ticket == 1 && req.key == 1 && req.coder == 3);
	assert(sim.next_ticket == 2);
	assert(free_sim(&sim) == 0);
}

/* AC2: EDF keys use the coder's last start plus burnout time. */
static void test_ac2_edf_requests(void)
{
	t_config config = get_config(MODE_EDF);
	t_sim sim;
	t_request req;

	assert(init_simulation(&sim, &config) == 0);
	assert(build_request(&sim.coders[1], &req) == 0);
	assert(req.ticket == 0 && req.key == 800 && req.coder == 2);
	assert(pthread_mutex_lock(&sim.coders[3].mutex) == 0);
	sim.coders[3].last_compile_start = 1500;
	assert(pthread_mutex_unlock(&sim.coders[3].mutex) == 0);
	assert(build_request(&sim.coders[3], &req) == 0);
	assert(req.ticket == 1 && req.key == 2300 && req.coder == 4);
	assert(free_sim(&sim) == 0);
}

/* AC3: building either policy leaves mutexes, heaps and coder state intact. */
static void test_ac3_no_queue_or_coder_mutation(void)
{
	t_config config = get_config(MODE_FIFO);
	t_sim sim;
	t_request req;
	int mode;
	int i;

	mode = 0;
	while (mode < 2)
	{
		config.mode = (t_mode)mode;
		assert(init_simulation(&sim, &config) == 0);
		assert(pthread_mutex_lock(&sim.coders[3].mutex) == 0);
		sim.coders[3].last_compile_start = 1500;
		assert(pthread_mutex_unlock(&sim.coders[3].mutex) == 0);
		if (mode == 0)
		{
			assert(build_request(&sim.coders[0], &req) == 0);
			assert(build_request(&sim.coders[2], &req) == 0);
		}
		else
		{
			assert(build_request(&sim.coders[1], &req) == 0);
			assert(build_request(&sim.coders[3], &req) == 0);
		}
		assert_unlocked(&sim.ticket_mutex);
		i = 0;
		while (i < 4)
		{
			assert_unlocked(&sim.coders[i].mutex);
			assert(sim.coders[i].last_compile_start == (i == 3 ? 1500 : 0));
			assert(sim.coders[i].compile_count == 0);
			assert(sim.dongles[i].heap.size == 0);
			i++;
		}
		assert(free_sim(&sim) == 0);
		mode++;
	}
}

/* AC4: invalid and overflowing EDF keys preserve all output bytes. */
static void test_ac4_reject_invalid_edf_key(void)
{
	t_config config = get_config(MODE_EDF);
	t_sim sim;
	t_request req;
	t_request before;
	long values[2] = {-5, LONG_MAX - 100};
	int i;

	assert(init_simulation(&sim, &config) == 0);
	i = 0;
	while (i < 2)
	{
		assert(pthread_mutex_lock(&sim.coders[0].mutex) == 0);
		sim.coders[0].last_compile_start = values[i];
		assert(pthread_mutex_unlock(&sim.coders[0].mutex) == 0);
		fill_sentinel(&req);
		memcpy(&before, &req, sizeof(req));
		assert(build_request(&sim.coders[0], &req) == -1);
		assert(memcmp(&req, &before, sizeof(req)) == 0);
		assert_unlocked(&sim.coders[0].mutex);
		i++;
	}
	assert(free_sim(&sim) == 0);
}

/* AC5: exhausted tickets fail without writing the request in both modes. */
static void test_ac5_reject_exhausted_tickets(void)
{
	t_config config = get_config(MODE_FIFO);
	t_sim sim;
	t_request req;
	t_request before;
	int mode;

	mode = 0;
	while (mode < 2)
	{
		config.mode = (t_mode)mode;
		assert(init_simulation(&sim, &config) == 0);
		sim.next_ticket = LONG_MAX;
		fill_sentinel(&req);
		memcpy(&before, &req, sizeof(req));
		assert(build_request(&sim.coders[0], &req) == -1);
		assert(memcmp(&req, &before, sizeof(req)) == 0);
		assert(sim.next_ticket == LONG_MAX);
		assert(free_sim(&sim) == 0);
		mode++;
	}
}

static void *build_worker_requests(void *arg)
{
	t_worker *worker = (t_worker *)arg;
	int i;

	i = 0;
	while (i < REQUESTS_PER_WORKER)
	{
		if (build_request(worker->code_owner, &worker->requests[i]) != 0)
		{
			worker->failed = 1;
			return (NULL);
		}
		i++;
	}
	return (NULL);
}

/* AC6: concurrent callers get distinct FIFO tickets with their own IDs. */
static void test_ac6_concurrent_requests(void)
{
	t_config config = get_config(MODE_FIFO);
	t_sim sim;
	t_worker workers[WORKERS];
	pthread_t threads[WORKERS];
	int seen[TOTAL_REQUESTS] = {0};
	int i;
	int j;
	long ticket;

	assert(init_simulation(&sim, &config) == 0);
	i = 0;
	while (i < WORKERS)
	{
		workers[i].code_owner = &sim.coders[i];
		workers[i].failed = 0;
		assert(pthread_create(&threads[i], NULL, build_worker_requests,
				&workers[i]) == 0);
		i++;
	}
	i = 0;
	while (i < WORKERS)
	{
		assert(pthread_join(threads[i], NULL) == 0);
		assert(workers[i].failed == 0);
		j = 0;
		while (j < REQUESTS_PER_WORKER)
		{
			ticket = workers[i].requests[j].ticket;
			assert(ticket >= 0 && ticket < TOTAL_REQUESTS);
			assert(seen[ticket] == 0);
			seen[ticket] = 1;
			assert(workers[i].requests[j].coder == sim.coders[i].id);
			assert(workers[i].requests[j].key == ticket);
			j++;
		}
		i++;
	}
	assert(sim.next_ticket == TOTAL_REQUESTS);
	assert(free_sim(&sim) == 0);
}

/* AC7: null input paths return errors and preserve output bytes. */
static void test_ac7_reject_null_inputs(void)
{
	t_config config = get_config(MODE_FIFO);
	t_sim sim;
	t_coder coder;
	t_request req;
	t_request before;

	fill_sentinel(&req);
	memcpy(&before, &req, sizeof(req));
	assert(build_request(NULL, &req) == -1);
	assert(memcmp(&req, &before, sizeof(req)) == 0);
	assert(init_simulation(&sim, &config) == 0);
	assert(build_request(&sim.coders[0], NULL) == -1);
	memset(&coder, 0, sizeof(coder));
	coder.sim = NULL;
	assert(build_request(&coder, &req) == -1);
	assert(memcmp(&req, &before, sizeof(req)) == 0);
	assert(sim.next_ticket == 0);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	test_ac1_fifo_requests();
	test_ac2_edf_requests();
	test_ac3_no_queue_or_coder_mutation();
	test_ac4_reject_invalid_edf_key();
	test_ac5_reject_exhausted_tickets();
	test_ac6_concurrent_requests();
	test_ac7_reject_null_inputs();
	return (0);
}
