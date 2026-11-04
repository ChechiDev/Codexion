#include "main.h"

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

int take_dongles(t_coder *coder, int *taken);

typedef struct s_call
{
	t_coder *coder;
	pthread_t thread;
	int result;
	int taken;
	int done;
} t_call;

static pthread_mutex_t g_observe = PTHREAD_MUTEX_INITIALIZER;
static int g_order[2];
static int g_order_count;

static t_config get_config(int count, t_mode mode, int cooldown)
{
	t_config c;

	c.number_of_coders = count;
	c.time_to_burnout = 800;
	c.time_to_compile = 200;
	c.time_to_debug = 200;
	c.time_to_refactor = 200;
	c.number_of_compiles_required = 3;
	c.dongle_cooldown = cooldown;
	c.mode = mode;
	return (c);
}

static void init_sim(t_sim *sim, int count, t_mode mode, int cooldown)
{
	t_config c = get_config(count, mode, cooldown);

	assert(init_simulation(sim, &c) == 0);
	sim->start_ms = get_time_ms();
	assert(sim->start_ms >= 0);
}

static void assert_unlocked(t_sim *sim)
{
	int i;

	for (i = 0; i < sim->config.number_of_coders; ++i)
	{
		assert(pthread_mutex_trylock(&sim->dongles[i].mutex) == 0);
		assert(pthread_mutex_unlock(&sim->dongles[i].mutex) == 0);
	}
}

static void *call_take(void *arg)
{
	t_call *call = (t_call *)arg;

	call->result = take_dongles(call->coder, &call->taken);
	assert(pthread_mutex_lock(&g_observe) == 0);
	call->done = 1;
	if (call->result == 0 && call->taken == 1 && g_order_count < 2)
		g_order[g_order_count++] = call->coder->id;
	assert(pthread_mutex_unlock(&g_observe) == 0);
	return (NULL);
}

static void start_call(t_call *call, t_coder *coder)
{
	call->coder = coder;
	call->result = -9;
	call->taken = -9;
	call->done = 0;
	assert(pthread_create(&call->thread, NULL, call_take, call) == 0);
}

static int has_done(t_call *call)
{
	int done;

	assert(pthread_mutex_lock(&g_observe) == 0);
	done = call->done;
	assert(pthread_mutex_unlock(&g_observe) == 0);
	return (done);
}

static void await_done(t_call *call)
{
	int i;

	for (i = 0; i < 1000 && !has_done(call); ++i)
		usleep(1000);
	assert(has_done(call));
	assert(pthread_join(call->thread, NULL) == 0);
}

static void await_heap_size(t_dongle *dongle, int size)
{
	int i;
	int actual = -1;

	for (i = 0; i < 1000; ++i)
	{
		assert(pthread_mutex_lock(&dongle->mutex) == 0);
		actual = dongle->heap.size;
		assert(pthread_mutex_unlock(&dongle->mutex) == 0);
		if (actual >= size)
			return;
		usleep(1000);
	}
	assert(actual >= size);
}

static void reset_order(void)
{
	assert(pthread_mutex_lock(&g_observe) == 0);
	g_order_count = 0;
	assert(pthread_mutex_unlock(&g_observe) == 0);
}

static void await_order_count(int count)
{
	int i;
	int actual = 0;

	for (i = 0; i < 1000; ++i)
	{
		assert(pthread_mutex_lock(&g_observe) == 0);
		actual = g_order_count;
		assert(pthread_mutex_unlock(&g_observe) == 0);
		if (actual >= count)
			return;
		usleep(1000);
	}
	assert(actual >= count);
}

/* AC1: free dongles are acquired together, with one ticket and no lock leak. */
static void test_ac1_free_pair(void)
{
	t_sim sim;
	int taken = -1;
	int i;

	init_sim(&sim, 4, MODE_FIFO, 50);
	assert(take_dongles(&sim.coders[0], &taken) == 0 && taken == 1);
	for (i = 0; i < 4; ++i)
	{
		assert(sim.dongles[i].available == (i < 2 ? 0 : 1));
		assert(sim.dongles[i].heap.size == 0);
		assert(sim.dongles[i].released_at == -1);
	}
	assert_unlocked(&sim);
	assert(sim.next_ticket == 1);
	assert(free_sim(&sim) == 0);
}

/* AC2: FIFO requires a request to head both dongle heaps before taking. */
static void test_ac2_head_of_both(void)
{
	t_sim sim;
	t_call second;
	t_call third;
	int taken = -1;

	init_sim(&sim, 4, MODE_FIFO, 50);
	reset_order();
	assert(take_dongles(&sim.coders[0], &taken) == 0 && taken == 1);
	start_call(&second, &sim.coders[1]);
	await_heap_size(&sim.dongles[1], 1);
	await_heap_size(&sim.dongles[2], 1);
	start_call(&third, &sim.coders[2]);
	await_heap_size(&sim.dongles[2], 2);
	await_heap_size(&sim.dongles[3], 1);
	assert(!has_done(&second) && !has_done(&third));
	assert(release_dongles(&sim.coders[0]) == 0);
	await_done(&second);
	assert(second.result == 0 && second.taken == 1);
	assert(!has_done(&third));
	assert(release_dongles(&sim.coders[1]) == 0);
	await_done(&third);
	assert(third.result == 0 && third.taken == 1);
	assert(g_order_count == 2 && g_order[0] == 2 && g_order[1] == 3);
	assert(release_dongles(&sim.coders[2]) == 0);
	assert(free_sim(&sim) == 0);
}

static void run_contended_order(t_mode mode, int expected_first)
{
	t_sim sim;
	t_call second;
	t_call third;
	t_call *winner;
	t_call *loser;

	init_sim(&sim, 4, mode, 0);
	reset_order();
	assert(pthread_mutex_lock(&sim.coders[1].mutex) == 0);
	sim.coders[1].last_compile_start = 300;
	assert(pthread_mutex_unlock(&sim.coders[1].mutex) == 0);
	assert(pthread_mutex_lock(&sim.dongles[2].mutex) == 0);
	sim.dongles[2].available = 0;
	assert(pthread_mutex_unlock(&sim.dongles[2].mutex) == 0);
	start_call(&second, &sim.coders[1]);
	await_heap_size(&sim.dongles[2], 1);
	start_call(&third, &sim.coders[2]);
	await_heap_size(&sim.dongles[2], 2);
	assert(pthread_mutex_lock(&sim.dongles[2].mutex) == 0);
	sim.dongles[2].available = 1;
	sim.dongles[2].released_at = -1;
	assert(pthread_cond_broadcast(&sim.dongles[2].cond) == 0);
	assert(pthread_mutex_unlock(&sim.dongles[2].mutex) == 0);
	winner = expected_first == 2 ? &second : &third;
	loser = expected_first == 2 ? &third : &second;
	await_order_count(1);
	assert(g_order[0] == expected_first);
	await_done(winner);
	assert(winner->result == 0 && winner->taken == 1);
	assert(!has_done(loser));
	assert(release_dongles(winner->coder) == 0);
	await_done(loser);
	assert(loser->result == 0 && loser->taken == 1);
	assert(g_order_count == 2 && g_order[1] == loser->coder->id);
	assert(release_dongles(loser->coder) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC3: FIFO grants first arrival; EDF grants the earlier burnout deadline. */
static void test_ac3_fifo_and_edf_order(void)
{
	run_contended_order(MODE_FIFO, 2);
	run_contended_order(MODE_EDF, 3);
}

/* AC4: a released dongle cannot be retaken before its cooldown expires. */
static void test_ac4_cooldown(void)
{
	t_sim sim;
	long released_at;
	long acquired_at;
	int taken = -1;

	init_sim(&sim, 4, MODE_FIFO, 50);
	assert(take_dongles(&sim.coders[0], &taken) == 0 && taken == 1);
	assert(release_dongles(&sim.coders[0]) == 0);
	released_at = sim.dongles[1].released_at;
	assert(take_dongles(&sim.coders[1], &taken) == 0 && taken == 1);
	acquired_at = get_elapsed_ms(sim.start_ms);
	assert(acquired_at >= released_at + 50);
	assert(acquired_at - released_at < 1000);
	assert(release_dongles(&sim.coders[1]) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC5: stop wakes a queued caller and prevents an already stopped caller taking. */
static void test_ac5_stop_while_waiting(void)
{
	t_sim sim;
	t_call second;
	int taken = -1;

	init_sim(&sim, 4, MODE_FIFO, 50);
	reset_order();
	assert(take_dongles(&sim.coders[0], &taken) == 0 && taken == 1);
	start_call(&second, &sim.coders[1]);
	await_heap_size(&sim.dongles[1], 1);
	assert(!has_done(&second));
	assert(set_stopped(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	await_done(&second);
	assert(second.result == 0 && second.taken == 0);
	assert(sim.dongles[2].available == 1);
	assert_unlocked(&sim);
	taken = -1;
	assert(take_dongles(&sim.coders[2], &taken) == 0 && taken == 0);
	assert(sim.dongles[3].available == 1);
	assert(free_sim(&sim) == 0);
}

typedef struct s_progress
{
	t_coder *coder;
	pthread_mutex_t *owners_mutex;
	int *owners;
	int completed;
} t_progress;

static void *run_progress(void *arg)
{
	t_progress *p = (t_progress *)arg;
	int i;
	int taken;
	int first = p->coder->first_dongle;
	int second = p->coder->second_dongle;

	for (i = 0; i < 50; ++i)
	{
		assert(take_dongles(p->coder, &taken) == 0 && taken == 1);
		assert(pthread_mutex_lock(p->owners_mutex) == 0);
		assert(p->owners[first] == 0 && p->owners[second] == 0);
		p->owners[first] = p->coder->id;
		p->owners[second] = p->coder->id;
		assert(pthread_mutex_unlock(p->owners_mutex) == 0);
		usleep(1000);
		assert(pthread_mutex_lock(p->owners_mutex) == 0);
		assert(p->owners[first] == p->coder->id);
		assert(p->owners[second] == p->coder->id);
		p->owners[first] = 0;
		p->owners[second] = 0;
		assert(pthread_mutex_unlock(p->owners_mutex) == 0);
		assert(release_dongles(p->coder) == 0);
		p->completed++;
	}
	return (NULL);
}

static void run_progress_mode(t_mode mode)
{
	t_sim sim;
	t_progress workers[5];
	pthread_t threads[5];
	pthread_mutex_t owners_mutex = PTHREAD_MUTEX_INITIALIZER;
	int owners[5] = {0, 0, 0, 0, 0};
	int i;
	long started;

	init_sim(&sim, 5, mode, 1);
	started = get_time_ms();
	for (i = 0; i < 5; ++i)
	{
		workers[i].coder = &sim.coders[i];
		workers[i].owners_mutex = &owners_mutex;
		workers[i].owners = owners;
		workers[i].completed = 0;
		assert(pthread_create(&threads[i], NULL, run_progress,
				&workers[i]) == 0);
	}
	for (i = 0; i < 5; ++i)
	{
		assert(pthread_join(threads[i], NULL) == 0);
		assert(workers[i].completed == 50);
	}
	for (i = 0; i < 5; ++i)
		assert(owners[i] == 0);
	assert(get_time_ms() - started < 10000);
	assert(pthread_mutex_destroy(&owners_mutex) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC6: repeated concurrent acquisition progresses without adjacent ownership. */
static void test_ac6_concurrent_progress(void)
{
	run_progress_mode(MODE_FIFO);
	run_progress_mode(MODE_EDF);
}

/* AC7: invalid inputs preserve the output and queue state. */
static void test_ac7_invalid_input(void)
{
	t_sim sim;
	t_sim *saved;
	int taken = 73;
	int i;

	init_sim(&sim, 4, MODE_FIFO, 50);
	assert(take_dongles(NULL, &taken) == -1 && taken == 73);
	assert(take_dongles(&sim.coders[0], NULL) == -1);
	saved = sim.coders[0].sim;
	sim.coders[0].sim = NULL;
	assert(take_dongles(&sim.coders[0], &taken) == -1 && taken == 73);
	sim.coders[0].sim = saved;
	assert_unlocked(&sim);
	for (i = 0; i < 4; ++i)
		assert(sim.dongles[i].heap.size == 0);
	assert(sim.next_ticket == 0);
	assert(free_sim(&sim) == 0);
	init_sim(&sim, 1, MODE_FIFO, 50);
	assert(take_dongles(&sim.coders[0], &taken) == -1 && taken == 73);
	assert(sim.dongles[0].heap.size == 0 && sim.next_ticket == 0);
	assert_unlocked(&sim);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	alarm(30);
	test_ac1_free_pair();
	test_ac2_head_of_both();
	test_ac3_fifo_and_edf_order();
	test_ac4_cooldown();
	test_ac5_stop_while_waiting();
	test_ac6_concurrent_progress();
	test_ac7_invalid_input();
	alarm(0);
	return (0);
}
