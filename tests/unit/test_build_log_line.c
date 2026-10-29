/* Unit tests for build_log_line: format, bounds and invalid input. */

#include <assert.h>
#include <limits.h>
#include <string.h>

#include "main.h"

static void	test_format(void)
{
	char	buf[LOG_LINE_MAX + 2];

	memset(buf, 'Q', sizeof(buf));
	assert(build_log_line(buf + 1, 12, 3, "burned out") == 16);
	assert(memcmp(buf + 1, "12 3 burned out\n", 16) == 0);
	assert(buf[0] == 'Q' && buf[17] == 'Q' && buf[LOG_LINE_MAX + 1] == 'Q');
}

static void	test_maximum(void)
{
	char	buf[LOG_LINE_MAX];
	char	text[LOG_TEXT_MAX + 1];

	memset(text, 'a', LOG_TEXT_MAX);
	text[LOG_TEXT_MAX] = '\0';
	assert(build_log_line(buf, LONG_MAX, INT_MAX, text) == LOG_LINE_MAX);
	assert(buf[LOG_LINE_MAX - 1] == '\n');
}

static void	test_invalid(void)
{
	char	buf[LOG_LINE_MAX];

	memset(buf, 'Q', sizeof(buf));
	assert(build_log_line(NULL, 0, 1, "x") == -1);
	assert(build_log_line(buf, 0, 1, NULL) == -1);
	assert(build_log_line(buf, -1, 1, "x") == -1);
	assert(build_log_line(buf, 0, 0, "x") == -1);
	assert(build_log_line(buf, 0, 1, "") == -1);
	assert(build_log_line(buf, 0, 1, "a\nb") == -1);
	assert(build_log_line(buf, 0, 1, "a\rb") == -1);
	assert(build_log_line(buf, 0, 1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == -1);
	assert(buf[0] == 'Q' && buf[LOG_LINE_MAX - 1] == 'Q');
}

int	main(void)
{
	test_format();
	test_maximum();
	test_invalid();
	return (0);
}
