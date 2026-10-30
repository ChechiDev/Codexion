# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sperez-l@student.42barcelona.com           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/26 16:47:34 by sperez-l          #+#    #+#              #
#    Updated: 2026/10/30 11:32:49 by sperez-l         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I.

SRC = main.c \
	src/cli/print_arg_error.c \
	src/cli/print_usage.c \
	src/heap/init_heap.c \
	src/log/build_log_line.c \
	src/log/print_burnout.c \
	src/log/print_log_line.c \
	src/log/print_state.c \
	src/parsing/get_arg_minimum.c \
	src/parsing/parse_config.c \
	src/parsing/parse_mode.c \
	src/parsing/parse_number.c \
	src/policy/get_request_key.c \
	src/policy/is_request_before.c \
	src/state/is_stopped.c \
	src/state/set_stopped.c \
	src/time/get_elapsed_ms.c \
	src/time/get_time_ms.c \
	src/time/sleep_ms.c \
	src/utils/build_number_text.c \
	src/utils/is_digit.c \
	src/utils/is_space.c \
	src/utils/trim_slice.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c main.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
