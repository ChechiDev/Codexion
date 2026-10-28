#define _POSIX_C_SOURCE 200809L

#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void init_sim(t_sim *sim)
{
	sim->start_ms = 0;
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
}

static void init_errorcheck_sim(t_sim *sim)
{
	pthread_mutexattr_t attr;

	assert(pthread_mutexattr_init(&attr) == 0);
	assert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
	sim->start_ms = 0;
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->log_mutex, &attr) == 0);
	assert(pthread_mutex_init(&sim->stop_mutex, &attr) == 0);
	assert(pthread_mutexattr_destroy(&attr) == 0);
}

static void destroy_sim(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
}

/* AC1: a successful stop changes the public guarded state to stopped. */
static void test_ac1_sets_guarded_stop_state(void)
{
	t_sim sim;

	init_sim(&sim);
	assert(is_stopped(&sim) == 0);
	assert(set_stopped(&sim) == 0);
	assert(is_stopped(&sim) == 1);
	destroy_sim(&sim);
}

/* AC2: stopping twice succeeds and leaves the state stopped. */
static void test_ac2_repeated_stop_is_idempotent(void)
{
	t_sim sim;

	init_sim(&sim);
	assert(set_stopped(&sim) == 0);
	assert(set_stopped(&sim) == 0);
	assert(is_stopped(&sim) == 1);
	assert(sim.stopped == 1);
	destroy_sim(&sim);
}

/* AC3: a successful stop releases both mutexes. */
static void test_ac3_releases_both_mutexes(void)
{
	t_sim sim;

	init_sim(&sim);
	assert(set_stopped(&sim) == 0);
	assert(pthread_mutex_trylock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_trylock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	destroy_sim(&sim);
}

/* AC4: failure to lock log_mutex preserves the flag and caller ownership. */
static void test_ac4_log_lock_failure_postcondition(void)
{
	t_sim sim;

	init_errorcheck_sim(&sim);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(set_stopped(&sim) == -1);
	assert(sim.stopped == 0);
	assert(pthread_mutex_trylock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	destroy_sim(&sim);
}

/* AC5: failure to lock stop_mutex releases log_mutex and preserves ownership. */
static void test_ac5_stop_lock_failure_postcondition(void)
{
	t_sim sim;

	init_errorcheck_sim(&sim);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(set_stopped(&sim) == -1);
	assert(sim.stopped == 0);
	assert(pthread_mutex_trylock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	destroy_sim(&sim);
}

static void assert_empty_file(FILE *file)
{
	assert(fflush(file) == 0);
	assert(fseek(file, 0, SEEK_END) == 0);
	assert(ftell(file) == 0);
}

/* AC6: success, repetition, and both lock failures write no output. */
static void test_ac6_prints_nothing(void)
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
	init_sim(&sim);
	assert(set_stopped(&sim) == 0);
	assert(set_stopped(&sim) == 0);
	destroy_sim(&sim);
	init_errorcheck_sim(&sim);
	assert(pthread_mutex_lock(&sim.log_mutex) == 0);
	assert(set_stopped(&sim) == -1);
	assert(pthread_mutex_unlock(&sim.log_mutex) == 0);
	assert(pthread_mutex_lock(&sim.stop_mutex) == 0);
	assert(set_stopped(&sim) == -1);
	assert(pthread_mutex_unlock(&sim.stop_mutex) == 0);
	destroy_sim(&sim);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) >= 0);
	assert(dup2(saved_err, STDERR_FILENO) >= 0);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	assert_empty_file(out);
	assert_empty_file(err);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC7: deliverable builds, exports set_stopped, and passes Norm. */
static void test_ac7_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]set_stopped$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_sets_guarded_stop_state();
	test_ac2_repeated_stop_is_idempotent();
	test_ac3_releases_both_mutexes();
	test_ac4_log_lock_failure_postcondition();
	test_ac5_stop_lock_failure_postcondition();
	test_ac6_prints_nothing();
	test_ac7_build_symbol_and_norm();
	return (0);
}
