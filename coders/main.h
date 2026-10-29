/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 18:02:34 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/29 18:37:25 by sperez-l         ###   ########.fr       */
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

typedef struct s_sim
{
	long			start_ms;
	int				stopped;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
}	t_sim;

typedef struct s_request
{
	long	key;
	long	ticket;
	int		coder;
}	t_request;

/* ---- Utils ---- */

int		is_digit(char c);
int		is_space(char c);
t_slice	trim_slice(const char *str);
int		build_number_text(long n, char *buf);

/* ---- Parsing ---- */

int		get_arg_minimum(int position);
int		parse_number(const char *str, int *out);
int		parse_mode(const char *str, t_mode *out);
int		parse_config(char **argv, t_config *config);

/* ---- Time ---- */

long	get_time_ms(void);
long	get_elapsed_ms(long start_ms);
int		sleep_ms(t_sim *sim, long ms);

/* ---- State ---- */

int		is_stopped(t_sim *sim);
int		set_stopped(t_sim *sim);

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

/* ---- CLI ---- */

void	print_usage(void);
void	print_arg_error(int position);

#endif
