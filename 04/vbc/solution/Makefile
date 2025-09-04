# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/03 20:44:51 by vpoka             #+#    #+#              #
#    Updated: 2025/09/03 20:44:52 by vpoka            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = vbc

SRCS = $(wildcard srcs/*.c)

OBJ_DIR = objs

OBJS = $(SRCS:srcs/%.c=$(OBJ_DIR)/%.o)

CC = cc

CFLAGS = -Wall -Wextra -Werror -g -Iinclude

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: srcs/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug