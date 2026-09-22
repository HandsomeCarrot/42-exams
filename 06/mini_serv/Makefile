NAME := test

CC := cc
CFLAGS := -fsanitize=address,undefined,leak -g -Wall -Wextra -Werror
INCLUDE := -Icreated
COMP := $(CC) $(CFLAGS) $(INCLUDE)

SRCS :=	created/mini_serv.c

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

.PHONY: all clean fclean re
