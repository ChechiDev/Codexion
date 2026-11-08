/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 18:02:34 by sperez-l          #+#    #+#             */
/*   Updated: 2026/11/08 11:37:12 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include <pthread.h>

/* ---- Macros ---- */

# define EXIT_USAGE 1
# define SLEEP_SLICE_US 500
# define NUMBER_TEXT_MAX 20
# define LOG_TEXT_MAX 32
# define LOG_LINE_MAX 64

/* ---- Types ---- */

typedef enum e_state
{
	STATE_TAKEN_DONGLE,
	STATE_COMPILING,
	STATE_DEBUGGING,
	STATE_REFACTORING
}	t_state;

typedef enum e_mode
{
	MODE_FIFO,
	MODE_EDF
}	t_mode;

typedef struct s_config
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	t_mode	mode;
}	t_config;

typedef struct s_slice
{
	const char	*start;
	int			len;
}	t_slice;

typedef struct s_request
{
	long	key;
	long	ticket;
	int		coder;
}	t_request;

typedef struct s_heap
{
	t_request	*items;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_heap			heap;
	int				available;
	long			released_at;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				first_dongle;
	int				second_dongle;
	pthread_mutex_t	mutex;
	long			last_compile_start;
	int				compile_count;
	pthread_t		thread;
	struct s_sim	*sim;
}	t_coder;

typedef struct s_sim
{
	t_config		config;
	long			start_ms;
	int				stopped;
	int				failed;
	pthread_t		monitor;
	long			next_ticket;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	ticket_mutex;
}	t_sim;

/* ---- Utils ---- */

int		is_digit(char c);
int		is_space(char c);
t_slice	trim_slice(const char *str);
int		build_number_text(long n, char *buf);

/* ---- Parsing ---- */

int		get_arg_minimum(int position);
int		parse_number(const char *str, int *out);
int		parse_mode(const char *str, t_mode *out);
int		parse_config(char **argv, t_config *config, int *invalid);

/* ---- Time ---- */

long	get_time_ms(void);
long	get_elapsed_ms(long start_ms);
int		get_deadline(long ms, struct timespec *out);
int		sleep_ms(t_sim *sim, long ms);

/* ---- State ---- */

int		is_stopped(t_sim *sim);
int		set_failed(t_sim *sim);
int		set_stopped(t_sim *sim);
int		take_ticket(t_sim *sim, long *out);

/* ---- Log ---- */

int		build_log_line(char *line, long ts, int coder, const char *text);
int		print_state(t_sim *sim, int coder, t_state state);
int		print_burnout(t_sim *sim, int coder);

/*
** Caller holds log_mutex. Reads the timestamp, builds the line with
** build_log_line and sends it with exactly one write, no retry.
** Returns 0 only if write returned len; -1 on clock, build or write
** failure. Never locks/unlocks a mutex, never reads or changes stopped.
*/
int		print_log_line(t_sim *sim, int coder, const char *text);

/* ---- Policy ---- */

int		is_request_before(const t_request *a, const t_request *b);
long	get_request_key(t_mode mode, long ticket, long last_compile_start,
			int time_to_burnout);
int		is_dongle_ready(const t_dongle *dongle, int coder, long now,
			int cooldown);

/* ---- Heap ---- */

int		init_heap(t_heap *heap, int capacity);
void	free_heap(t_heap *heap);
int		push_request(t_heap *heap, const t_request *req);
int		get_head_request(const t_heap *heap, t_request *out);
int		pop_request(t_heap *heap, t_request *out);

/* ---- Init ---- */

int		get_dongle_pair(int seat, int n, int *first, int *second);
int		init_dongle(t_dongle *dongle);
int		init_dongles(t_dongle **out, int n);
int		init_coder(t_coder *coder, t_sim *sim, int seat, int n);
int		init_coders(t_coder **out, t_sim *sim, int n);
int		init_simulation(t_sim *sim, const t_config *config);

/* ---- Cleanup ---- */

int		destroy_coder(t_coder *coder);
int		destroy_dongle(t_dongle *dongle);
int		free_dongles(t_dongle *dongles, int count);
int		free_coders(t_coder *coders, int count);
int		free_sim(t_sim *sim);

/* ---- Dongle ---- */

int		build_request(t_coder *coder, t_request *out);
int		wake_dongles(t_sim *sim);
int		wait_for_dongles(t_coder *coder);
int		release_dongles(t_coder *coder);
int		take_dongles(t_coder *coder, int *taken);
int		take_single_dongle(t_coder *coder, int *taken);

/* ---- Coder ---- */

int		set_compile_start(t_coder *coder);
int		set_compile_count(t_coder *coder);
int		take_coder_dongles(t_coder *coder, int *taken);
void	*run_coder(void *arg);

/* ---- Monitor ---- */

int		get_burned_out(t_coder *coder, int *burned);
int		get_target_met(t_sim *sim, int *met);
void	*run_monitor(void *arg);

/* ---- CLI ---- */

void	print_usage(void);
void	print_arg_error(int position);

#endif
