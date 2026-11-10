#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUTPUT_CAP 65536

typedef struct s_result
{
	int status;
	size_t out_len;
	size_t err_len;
	char out[OUTPUT_CAP];
	char err[OUTPUT_CAP];
} t_result;

static const char *get_binary(void)
{
	const char *path;

	path = getenv("CODEXION_BIN");
	if (path == NULL || *path == '\0')
		return ("coders/codexion");
	return (path);
}

static int get_slack(void)
{
	const char *value;
	char *end;
	long slack;

	value = getenv("CODEXION_TIME_SLACK_MS");
	if (value == NULL || *value == '\0')
		return (0);
	errno = 0;
	slack = strtol(value, &end, 10);
	assert(errno == 0 && *end == '\0' && slack >= 0 && slack <= 1000);
	return ((int)slack);
}

static size_t read_output(FILE *file, char *buffer)
{
	size_t length;

	assert(fseek(file, 0, SEEK_SET) == 0);
	length = fread(buffer, 1, OUTPUT_CAP - 1, file);
	assert(!ferror(file) && fgetc(file) == EOF && !ferror(file));
	buffer[length] = '\0';
	assert(memchr(buffer, '\0', length) == NULL);
	return (length);
}

static void run_case(int under_valgrind, const char *const args[], t_result *result)
{
	FILE *out;
	FILE *err;
	pid_t child;
	pid_t waited;
	int status;
	char *command[20];
	int i;
	int j;

	out = tmpfile();
	err = tmpfile();
	assert(out != NULL && err != NULL);
	child = fork();
	assert(child >= 0);
	if (child == 0)
	{
		alarm(under_valgrind ? 120 : 20);
		if (dup2(fileno(out), STDOUT_FILENO) < 0
			|| dup2(fileno(err), STDERR_FILENO) < 0)
			_exit(126);
		i = 0;
		if (under_valgrind)
		{
			command[i++] = (char *)"valgrind";
			command[i++] = (char *)"--leak-check=full";
			command[i++] = (char *)"--show-leak-kinds=all";
			command[i++] = (char *)"--errors-for-leak-kinds=all";
			command[i++] = (char *)"--error-exitcode=42";
		}
		command[i++] = (char *)get_binary();
		j = 0;
		while (args[j] != NULL)
			command[i++] = (char *)args[j++];
		command[i] = NULL;
		if (under_valgrind)
			execvp("valgrind", command);
		else
			execv(get_binary(), command);
		_exit(127);
	}
	do
	{
		waited = waitpid(child, &status, 0);
	} while (waited < 0 && errno == EINTR);
	assert(waited == child && WIFEXITED(status));
	result->status = WEXITSTATUS(status);
	result->out_len = read_output(out, result->out);
	result->err_len = read_output(err, result->err);
	assert(fclose(out) == 0 && fclose(err) == 0);
}

static int is_allowed_symbol(const char *name)
{
	static const char *allowed[] = {
		"pthread_create", "pthread_join", "pthread_mutex_init",
		"pthread_mutex_lock", "pthread_mutex_unlock",
		"pthread_mutex_destroy", "pthread_cond_init", "pthread_cond_wait",
		"pthread_cond_timedwait", "pthread_cond_broadcast",
		"pthread_cond_destroy", "gettimeofday", "usleep", "write",
		"malloc", "free", "printf", "fprintf", "strcmp", "strlen",
		"atoi", "memset", "_ITM_deregisterTMCloneTable",
		"_ITM_registerTMCloneTable", "__cxa_finalize", "__gmon_start__",
		"__libc_start_main", "__stack_chk_fail"
	};
	size_t i;
	size_t length;

	for (i = 0; i < sizeof(allowed) / sizeof(allowed[0]); i++)
	{
		length = strlen(allowed[i]);
		if (strncmp(name, allowed[i], length) == 0
			&& (name[length] == '\0' || name[length] == '@'))
			return (1);
	}
	return (0);
}

/* AC1: every undefined binary symbol is subject-allowed or toolchain startup. */
static void test_ac1_allowed_functions(void)
{
	FILE *symbols;
	char line[512];
	char type;
	char name[256];
	int status;
	int found;

	symbols = popen("nm -u coders/codexion", "r");
	assert(symbols != NULL);
	found = 0;
	while (fgets(line, sizeof(line), symbols) != NULL)
	{
		assert(strchr(line, '\n') != NULL || feof(symbols));
		assert(sscanf(line, " %c %255s", &type, name) == 2);
		assert(type == 'U' || type == 'w' || type == 'v');
		assert(is_allowed_symbol(name));
		found++;
	}
	assert(!ferror(symbols) && found > 0);
	status = pclose(symbols);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

static int is_local_static(const char *name)
{
	const char *suffix;

	if (name[0] == '.')
		return (1);
	suffix = strrchr(name, '.');
	if (suffix == NULL || suffix == name || suffix[1] == '\0')
		return (0);
	suffix++;
	while (*suffix != '\0')
	{
		if (!isdigit((unsigned char)*suffix))
			return (0);
		suffix++;
	}
	return (1);
}

/* AC2: object files define no file-scope data symbols, including local ones. */
static void test_ac2_no_globals(void)
{
	FILE *symbols;
	char line[512];
	char name[256];
	char type;
	unsigned long address;
	int status;
	int found;

	symbols = popen("nm -a --defined-only coders/main.o coders/src/*/*.o", "r");
	assert(symbols != NULL);
	found = 0;
	while (fgets(line, sizeof(line), symbols) != NULL)
	{
		assert(strchr(line, '\n') != NULL || feof(symbols));
		if (sscanf(line, "%lx %c %255s", &address, &type, name) != 3)
			continue;
		found++;
		if (strchr("BDCGSRbdcgsr", type) != NULL)
			assert(is_local_static(name));
	}
	assert(!ferror(symbols) && found > 0);
	status = pclose(symbols);
	assert(status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* AC3: the impossible run ends with exactly one burnout at 410..420+S ms. */
static void test_ac3_impossible_run(void)
{
	static const char *const args[] = {
		"4", "410", "200", "200", "200", "3", "0", "edf", NULL
	};
	t_result result;
	char *cursor;
	char *line_end;
	char *end;
	long timestamp;
	int burnouts;

	run_case(0, args, &result);
	assert(result.status == 0 && result.err_len == 0);
	assert(result.out_len > 0);
	cursor = result.out;
	burnouts = 0;
	while (*cursor != '\0')
	{
		line_end = strchr(cursor, '\n');
		assert(line_end != NULL);
		if (line_end - cursor >= 11
			&& memcmp(line_end - 11, " burned out\n", 12) == 0)
		{
			errno = 0;
			timestamp = strtol(cursor, &end, 10);
			assert(errno == 0 && end > cursor && *end == ' ');
			assert(timestamp >= 410 && timestamp <= 420 + get_slack());
			assert(line_end[1] == '\0');
			burnouts++;
		}
		cursor = line_end + 1;
	}
	assert(burnouts == 1);
}

/* AC4: invalid numeric input exits 1, with error and usage on stderr only. */
static void test_ac4_invalid_input(void)
{
	static const char *const args[] = {
		"4", "410", "200", "x", "200", "3", "0", "edf", NULL
	};
	t_result result;

	run_case(0, args, &result);
	assert(result.status == 1 && result.out_len == 0);
	assert(result.err_len > 0);
	assert(strncmp(result.err, "Error:", 6) == 0);
	assert(strstr(result.err, "Usage: ./codexion") != NULL);
}

/* AC5: Valgrind runs two valid cases and the invalid case without leaks. */
static void test_ac5_valgrind_runs(void)
{
	static const char *const single[] = {
		"1", "300", "200", "200", "200", "3", "0", "edf", NULL
	};
	static const char *const many[] = {
		"5", "5000", "200", "200", "200", "2", "0", "edf", NULL
	};
	static const char *const invalid[] = {
		"4", "410", "200", "x", "200", "3", "0", "edf", NULL
	};
	t_result result;
	int available;

	available = system("command -v valgrind >/dev/null 2>&1");
	assert(available != -1 && WIFEXITED(available));
	if (WEXITSTATUS(available) != 0)
	{
		fprintf(stderr, "SKIP AC5: valgrind is not installed\n");
		return ;
	}
	run_case(1, single, &result);
	assert(result.status == 0);
	run_case(1, many, &result);
	assert(result.status == 0);
	run_case(1, invalid, &result);
	assert(result.status == 1);
}

int main(void)
{
	test_ac1_allowed_functions();
	test_ac2_no_globals();
	test_ac3_impossible_run();
	test_ac4_invalid_input();
	test_ac5_valgrind_runs();
	return (0);
}
