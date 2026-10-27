# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sperez-l@student.42barcelona.com           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/26 16:47:34 by sperez-l          #+#    #+#              #
#    Updated: 2026/10/26 18:02:34 by sperez-l         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I.

SRC = main.c \
	src/parsing/trim_span.c

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
