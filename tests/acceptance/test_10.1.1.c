#define _GNU_SOURCE

#include "main.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

static t_sim *clock_stop_sim;
static int clock_fail;
static int clock_calls;

int gettimeofday(struct timeval *tv, void *tz)
{
	clock_calls++;
	if (clock_fail)
	{
		errno = EIO;
		return (-1);
	}
	if (clock_stop_sim != NULL)
	{
		t_sim *sim = clock_stop_sim;

		clock_stop_sim = NULL;
		assert(set_stopped(sim) == 0);
	}
	return ((int)syscall(SYS_gettimeofday, tv, tz));
}

static void init_case(t_sim *sim, t_coder *coder, int id)
{
	memset(sim, 0, sizeof(*sim));
	memset(coder, 0, sizeof(*coder));
	assert(pthread_mutex_init(&sim->stop_mutex, NULL) == 0);
	coder->id = id;
	coder->sim = sim;
	clock_stop_sim = NULL;
	clock_fail = 0;
	clock_calls = 0;
}

static void finish_case(t_sim *sim)
{
	assert(pthread_mutex_destroy(&sim->stop_mutex) == 0);
}

/* AC1: odd seats return zero without reading the clock, even when stopped. */
static void test_ac1_odd_seat_no_sleep(void)
{
	t_sim sim;
	t_coder coder;

	init_case(&sim, &coder, 1);
	assert(sleep_start_turn(&coder) == 0);
	assert(clock_calls == 0);
	coder.id = 3;
	assert(set_stopped(&sim) == 0);
	assert(sleep_start_turn(&coder) == 0);
	assert(clock_calls == 0);
	finish_case(&sim);
}

/* AC2: an even seat waits at least START_TURN_MS of clock time. */
static void test_ac2_even_seat_waits(void)
{
	t_sim sim;
	t_coder coder;
	long before;
	long after;

	init_case(&sim, &coder, 2);
	before = get_time_ms();
	assert(before >= 0);
	assert(sleep_start_turn(&coder) == 0);
	after = get_time_ms();
	assert(after >= before + START_TURN_MS);
	finish_case(&sim);
}

/* AC3: an even seat preserves sleep_ms's already-stopped result. */
static void test_ac3_even_seat_already_stopped(void)
{
	t_sim sim;
	t_coder coder;

	init_case(&sim, &coder, 4);
	assert(set_stopped(&sim) == 0);
	assert(sleep_start_turn(&coder) == 1);
	finish_case(&sim);
}

/* AC4: invalid coder or simulation pointers return an error. */
static void test_ac4_invalid_pointers(void)
{
	t_coder coder;

	memset(&coder, 0, sizeof(coder));
	assert(sleep_start_turn(NULL) == -1);
	coder.id = 2;
	assert(sleep_start_turn(&coder) == -1);
}

/* AC5: the offset produces no output and changes no compile or ticket state. */
static void test_ac5_no_output_or_state_change(void)
{
	t_sim sim;
	t_coder coder;
	FILE *out;
	FILE *err;
	int saved_out;
	int saved_err;

	init_case(&sim, &coder, 2);
	coder.last_compile_start = 37;
	coder.compile_count = 3;
	sim.next_ticket = 11;
	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	saved_out = dup(STDOUT_FILENO);
	saved_err = dup(STDERR_FILENO);
	assert(saved_out >= 0 && saved_err >= 0);
	assert(dup2(fileno(out), STDOUT_FILENO) == STDOUT_FILENO);
	assert(dup2(fileno(err), STDERR_FILENO) == STDERR_FILENO);
	assert(sleep_start_turn(&coder) == 0);
	assert(fflush(stdout) == 0 && fflush(stderr) == 0);
	assert(dup2(saved_out, STDOUT_FILENO) == STDOUT_FILENO);
	assert(dup2(saved_err, STDERR_FILENO) == STDERR_FILENO);
	assert(close(saved_out) == 0 && close(saved_err) == 0);
	assert(fseek(out, 0, SEEK_END) == 0 && ftell(out) == 0);
	assert(fseek(err, 0, SEEK_END) == 0 && ftell(err) == 0);
	assert(fclose(out) == 0 && fclose(err) == 0);
	assert(coder.last_compile_start == 37);
	assert(coder.compile_count == 3);
	assert(sim.next_ticket == 11);
	finish_case(&sim);
}

/* AC6: stopping on the first clock read interrupts the even-seat offset. */
static void test_ac6_stop_during_offset(void)
{
	t_sim sim;
	t_coder coder;

	init_case(&sim, &coder, 2);
	clock_stop_sim = &sim;
	assert(sleep_start_turn(&coder) == 1);
	assert(clock_calls >= 1);
	assert(sim.stopped == 1);
	finish_case(&sim);
}

/* AC7: a failed clock read is propagated as an error for an even seat. */
static void test_ac7_clock_failure(void)
{
	t_sim sim;
	t_coder coder;

	init_case(&sim, &coder, 2);
	clock_fail = 1;
	assert(sleep_start_turn(&coder) == -1);
	assert(clock_calls >= 1);
	clock_fail = 0;
	finish_case(&sim);
}

int main(void)
{
	test_ac1_odd_seat_no_sleep();
	test_ac2_even_seat_waits();
	test_ac3_even_seat_already_stopped();
	test_ac4_invalid_pointers();
	test_ac5_no_output_or_state_change();
	test_ac6_stop_during_offset();
	test_ac7_clock_failure();
	return (0);
}
