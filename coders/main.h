/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 18:02:34 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/28 18:45:22 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include <pthread.h>

/* ---- Macros ---- */

# define EXIT_USAGE 1
# define SLEEP_SLICE_US 500
# define NUMBER_TEXT_MAX 20

/* ---- Types ---- */

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

/* ---- CLI ---- */

void	print_usage(void);
void	print_arg_error(int position);

#endif
