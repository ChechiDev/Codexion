#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

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

static void init_full_sim(t_sim *sim)
{
	t_config config;

	config = get_config();
	assert(init_simulation(sim, &config) == 0);
}

static void init_errorcheck_sim(t_sim *sim)
{
	pthread_mutexattr_t attr;

	sim->failed = 0;
	assert(pthread_mutexattr_init(&attr) == 0);
	assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	assert(pthread_mutex_init(&sim->stop_mutex, &attr) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
}

static void destroy_errorcheck_sim(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
}

/* AC1: marking failure succeeds and does not stop an initialized run. */
static void test_ac1_marks_failure_without_stopping(void)
{
	t_sim sim;

	init_full_sim(&sim);
	assert(sim.failed == 0 && sim.stopped == 0);
	assert(set_failed(&sim) == 0);
	assert(sim.failed == 1 && sim.stopped == 0);
	assert(free_sim(&sim) == 0);
}

/* AC2: marking failure a second time succeeds and leaves it marked. */
static void test_ac2_repeated_mark_is_idempotent(void)
{
	t_sim sim;

	init_full_sim(&sim);
	assert(set_failed(&sim) == 0);
	assert(set_failed(&sim) == 0);
	assert(sim.failed == 1);
	assert(free_sim(&sim) == 0);
}

/* AC3: failure is still recorded after a normal stop. */
static void test_ac3_marks_failure_after_stop(void)
{
	t_sim sim;

	init_full_sim(&sim);
	assert(set_stopped(&sim) == 0);
	assert(set_failed(&sim) == 0);
	assert(sim.failed == 1 && sim.stopped == 1);
	assert(free_sim(&sim) == 0);
}

/* AC4: both state and log mutexes are unlocked after success. */
static void test_ac4_releases_mutexes_after_success(void)
{
	t_sim sim;

	init_full_sim(&sim);
	assert(set_failed(&sim) == 0);
	assert(pthread_mutex_trylock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_trylock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(free_sim(&sim) == 0);
}

typedef struct s_log_owner
{
	t_sim *sim;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int owned;
	int returned;
} t_log_owner;

static void *run_log_owner(void *arg)
{
	t_log_owner *owner;

	owner = arg;
	assert(pthread_mutex_lock(&owner->sim->log_mutex) == 0);
	assert(pthread_mutex_lock(&owner->mutex) == 0);
	owner->owned = 1;
	assert(pthread_cond_signal(&owner->cond) == 0);
	while (!owner->returned)
		assert(pthread_cond_wait(&owner->cond, &owner->mutex) == 0);
	assert(pthread_mutex_unlock(&owner->mutex) == 0);
	assert(pthread_mutex_unlock(&owner->sim->log_mutex) == 0);
	return (NULL);
}

/* AC5: set_failed finishes while another thread owns log_mutex. */
static void test_ac5_does_not_need_log_mutex(void)
{
	t_sim sim;
	t_log_owner owner;
	pthread_t thread;
	int result;

	init_full_sim(&sim);
	owner.sim = &sim;
	owner.owned = 0;
	owner.returned = 0;
	assert(pthread_mutex_init(&owner.mutex, NULL) == 0);
	assert(pthread_cond_init(&owner.cond, NULL) == 0);
	assert(pthread_create(&thread, NULL, run_log_owner, &owner) == 0);
	assert(pthread_mutex_lock(&owner.mutex) == 0);
	while (!owner.owned)
		assert(pthread_cond_wait(&owner.cond, &owner.mutex) == 0);
	assert(pthread_mutex_unlock(&owner.mutex) == 0);
	alarm(2);
	result = set_failed(&sim);
	assert(pthread_mutex_lock(&owner.mutex) == 0);
	owner.returned = 1;
	assert(pthread_cond_signal(&owner.cond) == 0);
	assert(pthread_mutex_unlock(&owner.mutex) == 0);
	assert(pthread_join(thread, NULL) == 0);
	alarm(0);
	assert(result == 0 && sim.failed == 1);
	assert(pthread_cond_destroy(&owner.cond) == 0);
	assert(pthread_mutex_destroy(&owner.mutex) == 0);
	assert(free_sim(&sim) == 0);
}

/* AC6: a failed stop_mutex relock preserves the flag and caller ownership. */
static void test_ac6_lock_failure_preserves_state(void)
{
	t_sim sim;

	init_errorcheck_sim(&sim);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(set_failed(&sim) == -1);
	assert(sim.failed == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	destroy_errorcheck_sim(&sim);
}

typedef struct s_start_gate
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int ready;
	int go;
} t_start_gate;

typedef struct s_worker
{
	t_sim *sim;
	t_start_gate *gate;
	int writer;
	int ok;
} t_worker;

static void *run_worker(void *arg)
{
	t_worker *worker;
	int i;
	int result;

	worker = arg;
	worker->ok = 1;
	assert(pthread_mutex_lock(&worker->gate->mutex) == 0);
	worker->gate->ready++;
	assert(pthread_cond_broadcast(&worker->gate->cond) == 0);
	while (!worker->gate->go)
		assert(pthread_cond_wait(&worker->gate->cond,
				&worker->gate->mutex) == 0);
	assert(pthread_mutex_unlock(&worker->gate->mutex) == 0);
	i = 0;
	while (i++ < 1000)
	{
		if (worker->writer)
			result = set_failed(worker->sim);
		else
			result = is_stopped(worker->sim);
		if (result != 0)
			worker->ok = 0;
	}
	return (NULL);
}

/* AC7: eight writers and eight readers share a start gate and finish cleanly. */
static void test_ac7_concurrent_writers_and_readers(void)
{
	t_sim sim;
	t_start_gate gate;
	t_worker workers[16];
	pthread_t threads[16];
	int i;

	init_full_sim(&sim);
	gate.ready = 0;
	gate.go = 0;
	assert(pthread_mutex_init(&gate.mutex, NULL) == 0);
	assert(pthread_cond_init(&gate.cond, NULL) == 0);
	alarm(10);
	i = 0;
	while (i < 16)
	{
		workers[i].sim = &sim;
		workers[i].gate = &gate;
		workers[i].writer = i < 8;
		assert(pthread_create(&threads[i], NULL, run_worker,
				&workers[i]) == 0);
		i++;
	}
	assert(pthread_mutex_lock(&gate.mutex) == 0);
	while (gate.ready != 16)
		assert(pthread_cond_wait(&gate.cond, &gate.mutex) == 0);
	gate.go = 1;
	assert(pthread_cond_broadcast(&gate.cond) == 0);
	assert(pthread_mutex_unlock(&gate.mutex) == 0);
	i = 0;
	while (i < 16)
	{
		assert(pthread_join(threads[i], NULL) == 0);
		assert(workers[i].ok == 1);
		i++;
	}
	alarm(0);
	assert(sim.failed == 1 && sim.stopped == 0);
	assert(pthread_cond_destroy(&gate.cond) == 0);
	assert(pthread_mutex_destroy(&gate.mutex) == 0);
	assert(free_sim(&sim) == 0);
}

static void assert_empty_file(FILE *file)
{
	assert(fflush(file) == 0);
	assert(fseek(file, 0, SEEK_END) == 0);
	assert(ftell(file) == 0);
}

/* AC8: successful, repeated, and failed calls print to neither stream. */
static void test_ac8_prints_nothing(void)
{
	t_sim sim;
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) >= 0);
	assert(dup2(fileno(err), STDERR_FILENO) >= 0);
	init_full_sim(&sim);
	assert(set_failed(&sim) == 0);
	assert(set_failed(&sim) == 0);
	assert(free_sim(&sim) == 0);
	init_errorcheck_sim(&sim);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(set_failed(&sim) == -1);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	destroy_errorcheck_sim(&sim);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(saved_err, STDERR_FILENO) >= 0);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	assert_empty_file(out);
	assert_empty_file(err);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

int main(void)
{
	test_ac1_marks_failure_without_stopping();
	test_ac2_repeated_mark_is_idempotent();
	test_ac3_marks_failure_after_stop();
	test_ac4_releases_mutexes_after_success();
	test_ac5_does_not_need_log_mutex();
	test_ac6_lock_failure_preserves_state();
	test_ac7_concurrent_writers_and_readers();
	test_ac8_prints_nothing();
	return (0);
}
