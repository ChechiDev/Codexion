#include "main.h"

#include <assert.h>
#include <pthread.h>
#include <unistd.h>

int take_single_dongle(t_coder *coder, int *taken);

typedef struct s_call
{
	t_coder *coder;
	pthread_t thread;
	int result;
	int taken;
	int done;
	long finished_at;
} t_call;

static pthread_mutex_t g_observe = PTHREAD_MUTEX_INITIALIZER;

static t_config get_config(int count, t_mode mode)
{
	t_config config;

	config.number_of_coders = count;
	config.time_to_burnout = 800;
	config.time_to_compile = 200;
	config.time_to_debug = 200;
	config.time_to_refactor = 200;
	config.number_of_compiles_required = 3;
	config.dongle_cooldown = 50;
	config.mode = mode;
	return (config);
}

static void init_sim(t_sim *sim, int count, t_mode mode)
{
	t_config config = get_config(count, mode);

	assert(init_simulation(sim, &config) == 0);
	sim->start_ms = get_time_ms();
	assert(sim->start_ms >= 0);
}

static void assert_unlocked(t_dongle *dongle)
{
	assert(pthread_mutex_trylock(&dongle->mutex) == 0);
	assert(pthread_mutex_unlock(&dongle->mutex) == 0);
}

static void *call_take(void *arg)
{
	t_call *call = (t_call *)arg;
	int result;
	long finished_at;

	result = take_single_dongle(call->coder, &call->taken);
	finished_at = get_elapsed_ms(call->coder->sim->start_ms);
	assert(pthread_mutex_lock(&g_observe) == 0);
	call->result = result;
	call->finished_at = finished_at;
	call->done = 1;
	assert(pthread_mutex_unlock(&g_observe) == 0);
	return (NULL);
}

static void start_call(t_call *call, t_coder *coder)
{
	call->coder = coder;
	call->result = -9;
	call->taken = -9;
	call->done = 0;
	call->finished_at = -1;
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

static void await_queued(t_dongle *dongle)
{
	int i;
	int size;

	size = 0;
	for (i = 0; i < 1000; ++i)
	{
		assert(pthread_mutex_lock(&dongle->mutex) == 0);
		size = dongle->heap.size;
		assert(pthread_mutex_unlock(&dongle->mutex) == 0);
		if (size == 1)
			return;
		usleep(1000);
	}
	assert(size == 1);
}

static void set_unavailable(t_dongle *dongle)
{
	assert(pthread_mutex_lock(&dongle->mutex) == 0);
	dongle->available = 0;
	assert(pthread_mutex_unlock(&dongle->mutex) == 0);
}

static void set_available(t_dongle *dongle)
{
	assert(pthread_mutex_lock(&dongle->mutex) == 0);
	dongle->available = 1;
	dongle->released_at = -1;
	assert(pthread_cond_broadcast(&dongle->cond) == 0);
	assert(pthread_mutex_unlock(&dongle->mutex) == 0);
}

/* AC1: the lone coder takes one dongle with one request and no lock leak. */
static void test_ac1_takes_one_dongle(void)
{
	t_sim sim;
	int taken = -9;

	init_sim(&sim, 1, MODE_FIFO);
	assert(sim.coders[0].first_dongle == 0);
	assert(sim.coders[0].second_dongle == 0);
	assert(take_single_dongle(&sim.coders[0], &taken) == 0);
	assert(taken == 1);
	assert(sim.dongles[0].available == 0);
	assert(sim.dongles[0].heap.size == 0);
	assert_unlocked(&sim.dongles[0]);
	assert(sim.next_ticket == 1);
	assert(free_sim(&sim) == 0);
}

/* AC2: a second take waits until cooldown expires in both policies. */
static void test_ac2_waits_for_cooldown(void)
{
	t_sim sim;
	t_call call;
	long released_at;
	int taken;
	int mode;

	for (mode = MODE_FIFO; mode <= MODE_EDF; ++mode)
	{
		init_sim(&sim, 1, (t_mode)mode);
		taken = -9;
		assert(take_single_dongle(&sim.coders[0], &taken) == 0);
		assert(taken == 1);
		assert(release_dongles(&sim.coders[0]) == 0);
		released_at = sim.dongles[0].released_at;
		assert(released_at >= 0);
		start_call(&call, &sim.coders[0]);
		await_done(&call);
		assert(call.result == 0 && call.taken == 1);
		assert(call.finished_at >= released_at + 50);
		assert(call.finished_at < released_at + 1000);
		assert(free_sim(&sim) == 0);
	}
}

/* AC3: an unavailable dongle holds exactly one queued request until freed. */
static void test_ac3_waits_with_one_request(void)
{
	t_sim sim;
	t_call call;
	t_dongle *dongle;

	init_sim(&sim, 1, MODE_FIFO);
	dongle = &sim.dongles[0];
	set_unavailable(dongle);
	start_call(&call, &sim.coders[0]);
	await_queued(dongle);
	assert(pthread_mutex_lock(&dongle->mutex) == 0);
	assert(dongle->heap.size == 1);
	assert(dongle->heap.items[0].coder == 1);
	assert(pthread_mutex_unlock(&dongle->mutex) == 0);
	assert(sim.next_ticket == 1);
	assert(!has_done(&call));
	set_available(dongle);
	await_done(&call);
	assert(call.result == 0 && call.taken == 1);
	assert(dongle->heap.size == 0);
	assert(free_sim(&sim) == 0);
}

/* AC4: stop wakes a queued request and an already stopped call takes nothing. */
static void test_ac4_stop_leaves_dongle_untaken(void)
{
	t_sim sim;
	t_call call;
	int taken;

	init_sim(&sim, 1, MODE_FIFO);
	set_unavailable(&sim.dongles[0]);
	start_call(&call, &sim.coders[0]);
	await_queued(&sim.dongles[0]);
	assert(!has_done(&call));
	assert(set_stopped(&sim) == 0);
	assert(wake_dongles(&sim) == 0);
	await_done(&call);
	assert(call.result == 0 && call.taken == 0);
	assert_unlocked(&sim.dongles[0]);
	assert(sim.dongles[0].available == 0);
	assert(free_sim(&sim) == 0);
	init_sim(&sim, 1, MODE_FIFO);
	assert(set_stopped(&sim) == 0);
	taken = -9;
	assert(take_single_dongle(&sim.coders[0], &taken) == 0);
	assert(taken == 0);
	assert(sim.dongles[0].available == 1);
	assert_unlocked(&sim.dongles[0]);
	assert(free_sim(&sim) == 0);
}

/* AC5: invalid inputs fail without changing output, queue, or lock state. */
static void test_ac5_rejects_invalid_input(void)
{
	t_sim sim;
	t_coder invalid;
	int taken;

	init_sim(&sim, 1, MODE_FIFO);
	taken = -9;
	assert(take_single_dongle(NULL, &taken) == -1 && taken == -9);
	assert(take_single_dongle(&sim.coders[0], NULL) == -1);
	invalid.sim = NULL;
	assert(take_single_dongle(&invalid, &taken) == -1 && taken == -9);
	assert(sim.dongles[0].heap.size == 0);
	assert_unlocked(&sim.dongles[0]);
	assert(free_sim(&sim) == 0);
	init_sim(&sim, 4, MODE_FIFO);
	assert(sim.coders[0].first_dongle != sim.coders[0].second_dongle);
	assert(take_single_dongle(&sim.coders[0], &taken) == -1 && taken == -9);
	assert(sim.next_ticket == 0);
	assert(sim.dongles[0].heap.size == 0);
	assert(sim.dongles[1].heap.size == 0);
	assert_unlocked(&sim.dongles[0]);
	assert_unlocked(&sim.dongles[1]);
	assert(free_sim(&sim) == 0);
}

int main(void)
{
	test_ac1_takes_one_dongle();
	test_ac2_waits_for_cooldown();
	test_ac3_waits_with_one_request();
	test_ac4_stop_leaves_dongle_untaken();
	test_ac5_rejects_invalid_input();
	return (0);
}
