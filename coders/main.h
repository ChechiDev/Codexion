/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sperez-l@student.42barcelona.com           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 18:02:34 by sperez-l          #+#    #+#             */
/*   Updated: 2026/10/27 17:31:16 by sperez-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

/* ---- Macros ---- */

# define EXIT_USAGE 1

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

typedef struct s_span
{
	const char	*start;
	int			len;
}	t_span;

/* ---- Utils ---- */

t_span	trim_span(const char *str);

/* ---- Parsing ---- */

int		parse_number(const char *str, int *out);

/* ---- CLI ---- */

void	print_usage(void);

#endif
