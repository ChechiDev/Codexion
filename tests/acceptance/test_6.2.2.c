#include "main.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static struct timeval get_wall_time(void)
{
	struct timeval	now;

	assert(gettimeofday(&now, NULL) == 0);
	return (now);
}

static struct timeval add_milliseconds(struct timeval value, long ms)
{
	value.tv_sec += ms / 1000;
	value.tv_usec += (ms % 1000) * 1000;
	if (value.tv_usec >= 1000000)
	{
		value.tv_sec++;
		value.tv_usec -= 1000000;
	}
	return (value);
}

static int compare_deadline(struct timespec deadline, struct timeval wall)
{
	if (deadline.tv_sec < wall.tv_sec)
		return (-1);
	if (deadline.tv_sec > wall.tv_sec)
		return (1);
	if (deadline.tv_nsec / 1000 < wall.tv_usec)
		return (-1);
	if (deadline.tv_nsec / 1000 > wall.tv_usec)
		return (1);
	return (0);
}

static void assert_deadline_bracket(long ms)
{
	struct timeval	before;
	struct timeval	after;
	struct timespec	deadline;

	before = get_wall_time();
	assert(get_deadline(ms, &deadline) == 0);
	after = get_wall_time();
	assert(deadline.tv_nsec % 1000 == 0);
	assert(compare_deadline(deadline, add_milliseconds(before, ms)) >= 0);
	assert(compare_deadline(deadline, add_milliseconds(after, ms)) <= 0);
}

/* AC1: a zero delay returns the current wall time at microsecond precision. */
static void test_ac1_zero_delay_bracket(void)
{
	assert_deadline_bracket(0);
}

/* AC2: short and second-crossing delays fit independent clock brackets. */
static void test_ac2_positive_delay_brackets(void)
{
	assert_deadline_bracket(1);
	assert_deadline_bracket(250);
	assert_deadline_bracket(1500);
}

/* AC3: all specified delays produce valid, normalised timespec values. */
static void test_ac3_normalised_output(void)
{
	const long		cases[] = {0, 1, 250, 1500};
	struct timespec	deadline;
	int				index;

	index = 0;
	while (index < 4)
	{
		assert(get_deadline(cases[index], &deadline) == 0);
		assert(deadline.tv_sec >= 0);
		assert(deadline.tv_nsec >= 0 && deadline.tv_nsec < 1000000000L);
		index++;
	}
	index = 0;
	while (index < 200)
	{
		assert(get_deadline(index * 10, &deadline) == 0);
		assert(deadline.tv_sec >= 0);
		assert(deadline.tv_nsec >= 0 && deadline.tv_nsec < 1000000000L);
		index++;
	}
}

/* AC4: an unsignalled condition wait times out near its 20 ms deadline. */
static void test_ac4_condition_wait_timeout(void)
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	struct timespec	deadline;
	struct timeval	before;
	struct timeval	after;
	long			elapsed_us;

	assert(pthread_mutex_init(&mutex, NULL) == 0);
	assert(pthread_cond_init(&cond, NULL) == 0);
	before = get_wall_time();
	assert(get_deadline(20, &deadline) == 0);
	assert(pthread_mutex_lock(&mutex) == 0);
	alarm(2);
	assert(pthread_cond_timedwait(&cond, &mutex, &deadline) == ETIMEDOUT);
	alarm(0);
	assert(pthread_mutex_unlock(&mutex) == 0);
	after = get_wall_time();
	elapsed_us = (after.tv_sec - before.tv_sec) * 1000000L
		+ after.tv_usec - before.tv_usec;
	assert(elapsed_us >= 20000 && elapsed_us < 1000000);
	assert(pthread_cond_destroy(&cond) == 0);
	assert(pthread_mutex_destroy(&mutex) == 0);
}

/* AC5: invalid delays and a null output fail without changing output bytes. */
static void test_ac5_invalid_input_preserves_output(void)
{
	struct timespec	deadline;
	struct timespec	original;

	memset(&deadline, 0x5a, sizeof(deadline));
	memcpy(&original, &deadline, sizeof(original));
	assert(get_deadline(-1, &deadline) == -1);
	assert(memcmp(&deadline, &original, sizeof(deadline)) == 0);
	assert(get_deadline(LONG_MIN, &deadline) == -1);
	assert(memcmp(&deadline, &original, sizeof(deadline)) == 0);
	assert(get_deadline(10, NULL) == -1);
}

/* AC6: LONG_MAX remains representable without a signed-overflow wrap. */
static void test_ac6_large_representable_deadline(void)
{
	struct timeval	before;
	struct timeval	after;
	struct timespec	deadline;
	long			seconds;

	before = get_wall_time();
	assert(get_deadline(LONG_MAX, &deadline) == 0);
	after = get_wall_time();
	seconds = LONG_MAX / 1000;
	assert(deadline.tv_nsec >= 0 && deadline.tv_nsec < 1000000000L);
	assert(deadline.tv_sec >= before.tv_sec + seconds);
	assert(deadline.tv_sec <= after.tv_sec + seconds + 1);
}

int	main(void)
{
	test_ac1_zero_delay_bracket();
	test_ac2_positive_delay_brackets();
	test_ac3_normalised_output();
	test_ac4_condition_wait_timeout();
	test_ac5_invalid_input_preserves_output();
	test_ac6_large_representable_deadline();
	return (0);
}
