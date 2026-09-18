NAME := test

CC := cc
CFLAGS := -fsanitize=address,undefined,leak -g -Wall -Wextra -Werror
INCLUDE := -Icreated
COMP := $(CC) $(CFLAGS) $(INCLUDE)

SRCS :=	created/main.c

B := build
OBJS := $(SRCS:%.c=$(B)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(COMP) $(OBJS) -o $(NAME)

$(B)/%.o: %.c
	@mkdir -p $(@D)
	$(COMP) -c $< -o $@

clean:
	rm -rf $(B)

fclean: clean
	rm -rf $(NAME)

re: fclean all

run: re
	@printf "\n----------\n\n"
	echo 'sdxddssaaww' | ./$(NAME) 5 5 0 | cat -e
	@printf "\n----------\n\n"
	echo 'sdxssdswdxddddsxaadwxwdxwaa' | ./$(NAME) 10 6 0 | cat -e
	@printf "\n----------\n\n"
	echo 'dxss' | ./$(NAME) 3 3 0 | cat -e
	@printf "\n----------\n\n"
	echo 'dxss' | ./$(NAME) 3 3 1 | cat -e
	@printf "\n----------\n\n"
	echo 'dxss' | ./$(NAME) 3 3 2 | cat -e

.PHONY: all clean fclean re run
