#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static void assert_line(long ts, int coder, const char *text,
	const char *expected, int length)
{
	unsigned char	guarded[66];
	int				index;

	memset(guarded, 'Q', sizeof(guarded));
	assert(build_log_line((char *)&guarded[1], ts, coder, text) == length);
	assert(guarded[0] == 'Q');
	index = 0;
	while (index < length)
	{
		assert(guarded[index + 1] == (unsigned char)expected[index]);
		index++;
	}
	while (index < 64)
	{
		assert(guarded[index + 1] == 'Q');
		index++;
	}
	assert(guarded[65] == 'Q');
}

/* AC1: the zero timestamp line has exact bytes and changes no other byte. */
static void test_ac1_zero_timestamp_exact_line(void)
{
	assert_line(0, 1, "is compiling", "0 1 is compiling\n", 17);
}

/* AC2: every subject state text has exact bytes and leaves the tail intact. */
static void test_ac2_subject_state_texts(void)
{
	const char	*texts[] = {"has taken a dongle", "is compiling",
		"is debugging", "is refactoring", "burned out"};
	const char	*expected[] = {"1204 3 has taken a dongle\n",
		"1204 3 is compiling\n", "1204 3 is debugging\n",
		"1204 3 is refactoring\n", "1204 3 burned out\n"};
	int			index;

	index = 0;
	while (index < 5)
	{
		assert_line(1204, 3, texts[index], expected[index],
			(int)strlen(expected[index]));
		index++;
	}
}

/* AC3: the maximum length fills exactly 64 bytes with no terminator. */
static void test_ac3_maximum_line_length(void)
{
	assert(sizeof(long) == 8);
	assert_line(LONG_MAX, INT_MAX, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		"9223372036854775807 2147483647 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n",
		64);
}

static void assert_invalid(long ts, int coder, const char *text)
{
	unsigned char	guarded[66];
	unsigned char	original[66];

	memset(guarded, 'Q', sizeof(guarded));
	memcpy(original, guarded, sizeof(guarded));
	assert(build_log_line((char *)&guarded[1], ts, coder, text) == -1);
	assert(memcmp(guarded, original, sizeof(guarded)) == 0);
}

/* AC4: invalid inputs fail without changing a destination or crashing. */
static void test_ac4_invalid_inputs_preserve_buffer(void)
{
	assert(build_log_line(NULL, 0, 1, "is compiling") == -1);
	assert(build_log_line(NULL, 0, 1, NULL) == -1);
	assert_invalid(0, 1, NULL);
	assert_invalid(-1, 1, "is compiling");
	assert_invalid(LONG_MIN, 1, "is compiling");
	assert_invalid(0, 0, "is compiling");
	assert_invalid(0, -5, "is compiling");
	assert_invalid(0, 1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	assert_invalid(0, 1, "");
	assert_invalid(0, 1, "is\ncompiling");
	assert_invalid(0, 1, "is\rcompiling");
}

/* AC5: public log capacities have the specified values. */
static void test_ac5_log_capacity_macros(void)
{
	assert(LOG_TEXT_MAX == 32);
	assert(LOG_LINE_MAX == 64);
}

static void run_command(const char *command)
{
	int	status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC6: the deliverable builds, exports build_log_line, and passes Norm. */
static void test_ac6_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]build_log_line$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_zero_timestamp_exact_line();
	test_ac2_subject_state_texts();
	test_ac3_maximum_line_length();
	test_ac4_invalid_inputs_preserve_buffer();
	test_ac5_log_capacity_macros();
	test_ac6_build_symbol_and_norm();
	return (0);
}
