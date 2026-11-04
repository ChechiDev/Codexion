# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sperez-l@student.42barcelona.com           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/26 16:47:34 by sperez-l          #+#    #+#              #
#    Updated: 2026/11/04 11:17:00 by sperez-l         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I.

SRC = main.c \
	src/cleanup/destroy_coder.c \
	src/cleanup/destroy_dongle.c \
	src/cleanup/free_coders.c \
	src/cleanup/free_dongles.c \
	src/cleanup/free_sim.c \
	src/cli/print_arg_error.c \
	src/cli/print_usage.c \
	src/dongle/build_request.c \
	src/dongle/wake_dongles.c \
	src/heap/free_heap.c \
	src/heap/get_head_request.c \
	src/heap/init_heap.c \
	src/heap/pop_request.c \
	src/heap/push_request.c \
	src/init/get_dongle_pair.c \
	src/init/init_coder.c \
	src/init/init_coders.c \
	src/init/init_dongle.c \
	src/init/init_dongles.c \
	src/init/init_simulation.c \
	src/log/build_log_line.c \
	src/log/print_burnout.c \
	src/log/print_log_line.c \
	src/log/print_state.c \
	src/parsing/get_arg_minimum.c \
	src/parsing/parse_config.c \
	src/parsing/parse_mode.c \
	src/parsing/parse_number.c \
	src/policy/get_request_key.c \
	src/policy/is_dongle_ready.c \
	src/policy/is_request_before.c \
	src/state/is_stopped.c \
	src/state/set_stopped.c \
	src/state/take_ticket.c \
	src/time/get_deadline.c \
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
