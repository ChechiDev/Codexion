#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/wait.h>

/* AC1: fifo returns each nonnegative ticket and ignores the time inputs. */
static void test_ac1_fifo_uses_ticket(void)
{
	long tickets[] = {0, 1, 42, LONG_MAX};
	long starts[] = {0, -1, LONG_MAX};
	int burnouts[] = {1, 0, -1};
	int i;
	int j;
	int k;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 3; j++)
			for (k = 0; k < 3; k++)
				assert(get_request_key(MODE_FIFO, tickets[i], starts[j],
					burnouts[k]) == tickets[i]);
}

/* AC2: edf computes a deadline independent of the arrival ticket. */
static void test_ac2_edf_deadline_ignores_ticket(void)
{
	long tickets[] = {0, 7, LONG_MAX};
	int i;

	assert(get_request_key(MODE_EDF, 3, 1000, 410) == 1410);
	assert(get_request_key(MODE_EDF, 0, 0, 1) == 1);
	for (i = 0; i < 3; i++)
		assert(get_request_key(MODE_EDF, tickets[i], 1000, 410)
			== 1410);
}

/* AC3: the largest representable deadline succeeds; overflow fails. */
static void test_ac3_edf_long_upper_bound(void)
{
	assert(get_request_key(MODE_EDF, 3, LONG_MAX - INT_MAX,
			INT_MAX) == LONG_MAX);
	assert(get_request_key(MODE_EDF, 3, LONG_MAX - INT_MAX + 1,
			INT_MAX) == -1);
	assert(get_request_key(MODE_EDF, 3, LONG_MAX, 1) == -1);
}

/* AC4: invalid ticket, edf time, burnout, or mode returns -1. */
static void test_ac4_invalid_input(void)
{
	assert(get_request_key(MODE_FIFO, -1, 1000, 410) == -1);
	assert(get_request_key(MODE_FIFO, LONG_MIN, 1000, 410) == -1);
	assert(get_request_key(MODE_EDF, -1, 1000, 410) == -1);
	assert(get_request_key(MODE_EDF, LONG_MIN, 1000, 410) == -1);
	assert(get_request_key(MODE_EDF, 3, -1, 410) == -1);
	assert(get_request_key(MODE_EDF, 3, LONG_MIN, 410) == -1);
	assert(get_request_key(MODE_EDF, 3, 1000, 0) == -1);
	assert(get_request_key(MODE_EDF, 3, 1000, -1) == -1);
	assert(get_request_key(MODE_EDF, 3, 1000, INT_MIN) == -1);
	assert(get_request_key((t_mode)7, 3, 1000, 410) == -1);
}

/* AC5: every valid AC1-AC3 call has a nonnegative key, distinct from -1. */
static void test_ac5_valid_keys_do_not_collide_with_error(void)
{
	long tickets[] = {0, 1, 42, LONG_MAX};
	long starts[] = {0, -1, LONG_MAX};
	int burnouts[] = {1, 0, -1};
	int i;
	int j;
	int k;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 3; j++)
			for (k = 0; k < 3; k++)
				assert(get_request_key(MODE_FIFO, tickets[i], starts[j],
					burnouts[k]) >= 0);
	assert(get_request_key(MODE_EDF, 3, 1000, 410) >= 0);
	assert(get_request_key(MODE_EDF, 0, 0, 1) >= 0);
	assert(get_request_key(MODE_EDF, 0, 1000, 410) >= 0);
	assert(get_request_key(MODE_EDF, 7, 1000, 410) >= 0);
	assert(get_request_key(MODE_EDF, LONG_MAX, 1000, 410) >= 0);
	assert(get_request_key(MODE_EDF, 3, LONG_MAX - INT_MAX,
			INT_MAX) >= 0);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC6: the deliverable builds, exports the function, and passes Norm. */
static void test_ac6_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]get_request_key$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_fifo_uses_ticket();
	test_ac2_edf_deadline_ignores_ticket();
	test_ac3_edf_long_upper_bound();
	test_ac4_invalid_input();
	test_ac5_valid_keys_do_not_collide_with_error();
	test_ac6_build_symbol_and_norm();
	return (0);
}
