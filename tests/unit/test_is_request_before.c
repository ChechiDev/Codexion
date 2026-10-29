#include "main.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>

int main(void)
{
	t_request a = {5, 9, 1};
	t_request b = {6, 1, 2};
	t_request c = {5, 10, 3};
	t_request lo = {LONG_MIN, LONG_MIN, 1};
	t_request hi = {LONG_MAX, LONG_MAX, 2};

	assert(is_request_before(&a, &b) == 1);
	assert(is_request_before(&b, &a) == 0);
	assert(is_request_before(&a, &c) == 1);
	assert(is_request_before(&c, &a) == 0);
	assert(is_request_before(&a, &a) == 0);
	assert(is_request_before(&lo, &hi) == 1);
	assert(is_request_before(&hi, &lo) == 0);
	assert(is_request_before(NULL, &a) == 0);
	assert(is_request_before(&a, NULL) == 0);
	assert(is_request_before(NULL, NULL) == 0);
	return (0);
}
