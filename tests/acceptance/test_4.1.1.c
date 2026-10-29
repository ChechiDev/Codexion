#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static t_request make_request(long key, long ticket, int coder)
{
	t_request request;

	memset(&request, 0, sizeof(request));
	request.key = key;
	request.ticket = ticket;
	request.coder = coder;
	return (request);
}

static int before_unchanged(const t_request *a, const t_request *b)
{
	t_request saved_a;
	t_request saved_b;
	int result;

	if (a != NULL)
		memcpy(&saved_a, a, sizeof(saved_a));
	if (b != NULL)
		memcpy(&saved_b, b, sizeof(saved_b));
	result = is_request_before(a, b);
	if (a != NULL)
		assert(memcmp(a, &saved_a, sizeof(saved_a)) == 0);
	if (b != NULL)
		assert(memcmp(b, &saved_b, sizeof(saved_b)) == 0);
	return (result);
}

/* AC1: smaller key wins regardless of ticket, in either argument order. */
static void test_ac1_smaller_key_first(void)
{
	t_request a = make_request(5, 9, 1);
	t_request b = make_request(6, 1, 2);

	assert(before_unchanged(&a, &b) == 1);
	assert(before_unchanged(&b, &a) == 0);
}

/* AC2: the smaller ticket wins when keys match, in either order. */
static void test_ac2_ticket_breaks_key_tie(void)
{
	t_request a = make_request(7, 2, 1);
	t_request b = make_request(7, 3, 2);

	assert(before_unchanged(&a, &b) == 1);
	assert(before_unchanged(&b, &a) == 0);
}

/* AC3: a request is never before itself or a copy of its ordering fields. */
static void test_ac3_strict_order(void)
{
	t_request a = make_request(7, 2, 1);
	t_request copy = make_request(7, 2, 1);

	assert(before_unchanged(&a, &a) == 0);
	assert(before_unchanged(&a, &copy) == 0);
	assert(before_unchanged(&copy, &a) == 0);
}

/* AC4: coder neither breaks an exact tie nor changes key/ticket order. */
static void test_ac4_coder_does_not_affect_order(void)
{
	t_request a = make_request(7, 2, 1);
	t_request b = make_request(7, 2, 2);
	t_request smaller_key = make_request(5, 9, 1);
	t_request larger_key = make_request(6, 1, 2);
	t_request smaller_ticket = make_request(7, 2, 1);
	t_request larger_ticket = make_request(7, 3, 2);

	assert(before_unchanged(&a, &b) == 0);
	assert(before_unchanged(&b, &a) == 0);
	assert(before_unchanged(&smaller_key, &larger_key) == 1);
	assert(before_unchanged(&larger_key, &smaller_key) == 0);
	assert(before_unchanged(&smaller_ticket, &larger_ticket) == 1);
	assert(before_unchanged(&larger_ticket, &smaller_ticket) == 0);
	smaller_key.coder = 2;
	larger_key.coder = 1;
	smaller_ticket.coder = 2;
	larger_ticket.coder = 1;
	assert(before_unchanged(&smaller_key, &larger_key) == 1);
	assert(before_unchanged(&larger_key, &smaller_key) == 0);
	assert(before_unchanged(&smaller_ticket, &larger_ticket) == 1);
	assert(before_unchanged(&larger_ticket, &smaller_ticket) == 0);
}

/* AC5: extreme signed long fields compare without overflow. */
static void test_ac5_extreme_values(void)
{
	t_request low_key = make_request(LONG_MIN, 0, 1);
	t_request high_key = make_request(LONG_MAX, 0, 2);
	t_request low_ticket = make_request(0, LONG_MIN, 1);
	t_request high_ticket = make_request(0, LONG_MAX, 2);
	t_request negative_key = make_request(-1, 5, 1);
	t_request zero_key = make_request(0, 0, 2);

	assert(before_unchanged(&low_key, &high_key) == 1);
	assert(before_unchanged(&high_key, &low_key) == 0);
	assert(before_unchanged(&low_ticket, &high_ticket) == 1);
	assert(before_unchanged(&high_ticket, &low_ticket) == 0);
	assert(before_unchanged(&negative_key, &zero_key) == 1);
}

/* AC6: eight distinct requests form a strict lexicographic total order. */
static void test_ac6_sample_total_order(void)
{
	t_request sample[8];
	int i;
	int j;
	int k;
	int expected;

	sample[0] = make_request(-9, 4, 1);
	sample[1] = make_request(0, -8, 2);
	sample[2] = make_request(5, 2, 3);
	sample[3] = make_request(-9, -1, 4);
	sample[4] = make_request(5, 1, 5);
	sample[5] = make_request(-1, 0, 6);
	sample[6] = make_request(0, 7, 7);
	sample[7] = make_request(9, -3, 8);
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			expected = sample[i].key < sample[j].key
				|| (sample[i].key == sample[j].key
					&& sample[i].ticket < sample[j].ticket);
			assert(before_unchanged(&sample[i], &sample[j]) == expected);
			if (i != j)
				assert(before_unchanged(&sample[i], &sample[j])
					+ before_unchanged(&sample[j], &sample[i]) == 1);
			for (k = 0; k < 8; k++)
				if (before_unchanged(&sample[i], &sample[j]) == 1
					&& before_unchanged(&sample[j], &sample[k]) == 1)
					assert(before_unchanged(&sample[i], &sample[k]) == 1);
		}
	}
}

/* AC7: either NULL argument, or both, returns zero without a crash. */
static void test_ac7_null_arguments(void)
{
	t_request a = make_request(1, 2, 3);

	assert(before_unchanged(NULL, &a) == 0);
	assert(before_unchanged(&a, NULL) == 0);
	assert(before_unchanged(NULL, NULL) == 0);
}

/* AC8: every comparison preserves both request byte representations. */
static void test_ac8_arguments_read_only(void)
{
	t_request a = make_request(LONG_MIN, LONG_MAX, 1);
	t_request b = make_request(LONG_MAX, LONG_MIN, 2);

	assert(before_unchanged(&a, &b) == 1);
	assert(before_unchanged(&b, &a) == 0);
	assert(before_unchanged(&a, &a) == 0);
}

static void run_command(const char *command)
{
	int status;

	status = system(command);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC9: deliverable builds, exports the comparator, and passes Norm. */
static void test_ac9_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]is_request_before$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_smaller_key_first();
	test_ac2_ticket_breaks_key_tie();
	test_ac3_strict_order();
	test_ac4_coder_does_not_affect_order();
	test_ac5_extreme_values();
	test_ac6_sample_total_order();
	test_ac7_null_arguments();
	test_ac8_arguments_read_only();
	test_ac9_build_symbol_and_norm();
	return (0);
}
