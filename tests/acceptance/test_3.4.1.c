#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static void assert_number(long value, const char *digits, int count)
{
	char	buffer[NUMBER_TEXT_MAX];

	memset(buffer, '?', sizeof(buffer));
	assert(build_number_text(value, buffer) == count);
	assert(memcmp(buffer, digits, (size_t)count) == 0);
}

/* AC1: canonical digits and counts across boundaries and buffer reuse. */
static void test_ac1_canonical_digits_and_reuse(void)
{
	char	buffer[NUMBER_TEXT_MAX];

	assert_number(0, "0", 1);
	assert_number(7, "7", 1);
	assert_number(9, "9", 1);
	assert_number(10, "10", 2);
	assert_number(42, "42", 2);
	assert_number(99, "99", 2);
	assert_number(100, "100", 3);
	assert_number(999999, "999999", 6);
	assert_number(1000000, "1000000", 7);
	memset(buffer, '?', sizeof(buffer));
	assert(build_number_text(1000000, buffer) == 7);
	assert(memcmp(buffer, "1000000", 7) == 0);
	assert(build_number_text(7, buffer) == 1);
	assert(buffer[0] == '7');
	assert(build_number_text(42, buffer) == 2);
	assert(memcmp(buffer, "42", 2) == 0);
}

/* AC2: LONG_MAX and its predecessor have the specified 19 decimal digits. */
static void test_ac2_long_max_digits(void)
{
	assert(sizeof(long) == 8);
	assert_number(LONG_MAX, "9223372036854775807", 19);
	assert_number(LONG_MAX - 1, "9223372036854775806", 19);
}

/* AC3: the public number-text capacity is exactly 20 bytes. */
static void test_ac3_number_text_capacity(void)
{
	assert(NUMBER_TEXT_MAX == 20);
}

static void assert_guarded_write(long value, const char *digits, int count)
{
	unsigned char	guarded[NUMBER_TEXT_MAX + 2];
	int				index;

	memset(guarded, 'Q', sizeof(guarded));
	assert(build_number_text(value, (char *)&guarded[1]) == count);
	assert(guarded[0] == 'Q');
	index = 0;
	while (index < count)
	{
		assert(guarded[index + 1] == (unsigned char)digits[index]);
		index++;
	}
	while (index < NUMBER_TEXT_MAX)
	{
		assert(guarded[index + 1] == 'Q');
		index++;
	}
	assert(guarded[NUMBER_TEXT_MAX + 1] == 'Q');
}

/* AC4: zero and LONG_MAX change only digit bytes within guarded storage. */
static void test_ac4_guard_bytes_and_no_terminator(void)
{
	assert_guarded_write(0, "0", 1);
	assert_guarded_write(LONG_MAX, "9223372036854775807", 19);
}

/* AC5: negative inputs fail without changing any guarded byte. */
static void test_ac5_negative_input_preserves_buffer(void)
{
	unsigned char	guarded[NUMBER_TEXT_MAX + 2];
	unsigned char	original[NUMBER_TEXT_MAX + 2];

	memset(guarded, 'Q', sizeof(guarded));
	memcpy(original, guarded, sizeof(guarded));
	assert(build_number_text(-1, (char *)&guarded[1]) == -1);
	assert(memcmp(guarded, original, sizeof(guarded)) == 0);
	assert(build_number_text(LONG_MIN, (char *)&guarded[1]) == -1);
	assert(memcmp(guarded, original, sizeof(guarded)) == 0);
}

/* AC6: a NULL destination fails safely. */
static void test_ac6_null_buffer_fails(void)
{
	assert(build_number_text(0, NULL) == -1);
}

static void run_command(const char *command)
{
	int	status;

	status = system(command);
	assert(status != -1);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == 0);
}

/* AC7: the deliverable builds, exports the helper, and passes Norm. */
static void test_ac7_build_symbol_and_norm(void)
{
	run_command("make -C coders >/dev/null 2>&1");
	run_command("nm coders/codexion | grep -Eq '[[:space:]]T[[:space:]]build_number_text$'");
	run_command("norminette coders/ >/dev/null 2>&1");
}

int main(void)
{
	test_ac1_canonical_digits_and_reuse();
	test_ac2_long_max_digits();
	test_ac3_number_text_capacity();
	test_ac4_guard_bytes_and_no_terminator();
	test_ac5_negative_input_preserves_buffer();
	test_ac6_null_buffer_fails();
	test_ac7_build_symbol_and_norm();
	return (0);
}
