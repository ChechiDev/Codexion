#define _DEFAULT_SOURCE

#include "main.h"

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct s_sleep_worker
{
	t_sim			*sim;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				ready;
	int				result;
	long			returned_ms;
} t_sleep_worker;

static long get_timing_slack_ms(void)
{
	const char	*value;
	char		*end;
	long		slack;

	value = getenv("CODEXION_TIME_SLACK_MS");
	if (value == NULL)
		return (0);
	slack = strtol(value, &end, 10);
	assert(end != value && *end == '\0' && slack >= 0);
	return (slack);
}

static void init_sim(t_sim *sim)
{
	sim->start_ms = 0;
	sim->stopped = 0;
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
	assert(pthread_mutex_init(&sim->log_mutex, NULL) == 0);
}

static void destroy_sim(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
	assert(pthread_mutex_destroy(&sim->log_mutex) == 0);
}

static void assert_immediate_sleep(t_sim *sim, long duration)
{
	long	before;
	long	after;

	before = get_time_ms();
	assert(before >= 0);
	assert(sleep_ms(sim, duration) == 0);
	after = get_time_ms();
	assert(after >= before && after - before < 2 + get_timing_slack_ms());
}

/* AC1: nonpositive durations return immediately, running or stopped. */
static void test_ac1_nonpositive_duration_returns_immediately(void)
{
	t_sim	sim;

	init_sim(&sim);
	assert_immediate_sleep(&sim, 0);
	assert_immediate_sleep(&sim, -1);
	assert(set_stopped(&sim) == 0);
	assert_immediate_sleep(&sim, 0);
	assert_immediate_sleep(&sim, -1);
	destroy_sim(&sim);
}

/* AC2: running sleep reaches 100 ms within the specified upper bound. */
static void test_ac2_running_sleep_reaches_duration(void)
{
	t_sim	sim;
	long	before;
	long	after;

	init_sim(&sim);
	before = get_time_ms();
	assert(before >= 0);
	assert(sleep_ms(&sim, 100) == 0);
	after = get_time_ms();
	assert(after - before >= 100);
	assert(after - before <= 110 + get_timing_slack_ms());
	destroy_sim(&sim);
}

/* AC3: an already stopped simulation interrupts a long sleep immediately. */
static void test_ac3_stopped_sleep_returns_immediately(void)
{
	t_sim	sim;
	long	before;
	long	after;

	init_sim(&sim);
	assert(set_stopped(&sim) == 0);
	before = get_time_ms();
	assert(before >= 0);
	assert(sleep_ms(&sim, 1000) == 1);
	after = get_time_ms();
	assert(after >= before && after - before < 2 + get_timing_slack_ms());
	destroy_sim(&sim);
}

static void *run_sleep_worker(void *arg)
{
	t_sleep_worker	*worker;

	worker = arg;
	assert(pthread_mutex_lock(&worker->mutex) == 0);
	worker->ready = 1;
	assert(pthread_cond_signal(&worker->cond) == 0);
	assert(pthread_mutex_unlock(&worker->mutex) == 0);
	worker->result = sleep_ms(worker->sim, 10000);
	worker->returned_ms = get_time_ms();
	return (NULL);
}

/* AC4: a concurrent stop interrupts sleep within 10 + S ms. */
static void test_ac4_concurrent_stop_interrupts_sleep(void)
{
	t_sim			sim;
	t_sleep_worker	worker;
	pthread_t		thread;
	long			stop_ms;

	init_sim(&sim);
	worker.sim = &sim;
	worker.ready = 0;
	assert(pthread_mutex_init(&worker.mutex, NULL) == 0);
	assert(pthread_cond_init(&worker.cond, NULL) == 0);
	assert(alarm(5) == 0);
	assert(pthread_create(&thread, NULL, run_sleep_worker, &worker) == 0);
	assert(pthread_mutex_lock(&worker.mutex) == 0);
	while (!worker.ready)
		assert(pthread_cond_wait(&worker.cond, &worker.mutex) == 0);
	assert(pthread_mutex_unlock(&worker.mutex) == 0);
	assert(usleep(50000) == 0);
	stop_ms = get_time_ms();
	assert(stop_ms >= 0);
	assert(set_stopped(&sim) == 0);
	assert(pthread_join(thread, NULL) == 0);
	assert(alarm(0) != 0);
	assert(worker.result == 1);
	assert(worker.returned_ms >= stop_ms);
	assert(worker.returned_ms <= stop_ms + 10 + get_timing_slack_ms());
	assert(pthread_cond_destroy(&worker.cond) == 0);
	assert(pthread_mutex_destroy(&worker.mutex) == 0);
	destroy_sim(&sim);
}

static void run_command(const char *command)
{
	int	status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC5: the deliverable builds, exports sleep_ms, and passes Norm. */
static void test_ac5_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]sleep_ms$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_nonpositive_duration_returns_immediately();
	test_ac2_running_sleep_reaches_duration();
	test_ac3_stopped_sleep_returns_immediately();
	test_ac4_concurrent_stop_interrupts_sleep();
	test_ac5_build_symbol_and_norm();
	return (0);
}
