NAME := mini_serv
SELECT_NAME := stack_select
POLL_NAME := stack_poll

CC := cc
CFLAGS := -Wall -Wextra -Werror -g -fsanitize=address,undefined,leak
INCLUDE := -Icreated
COMP := $(CC) $(CFLAGS) $(INCLUDE)

SRC := created/mini_serv.c
SELECT_SRC := created/mini_serv_select_stack.c
POLL_SRC := created/mini_serv_poll_stack.c

B := build
OBJ := $(SRC:%.c=$(B)/%.o)
SELECT_OBJ := $(SELECT_SRC:%.c=$(B)/%.o)
POLL_OBJ := $(POLL_SRC:%.c=$(B)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(COMP) $(OBJ) -o $(NAME)

$(SELECT_NAME): $(SELECT_OBJ)
	$(COMP) $(SELECT_OBJ) -o $(SELECT_NAME)

$(POLL_NAME): $(POLL_OBJ)
	$(COMP) $(POLL_OBJ) -o $(POLL_NAME)

$(B)/%.o: %.c
	@mkdir -p $(@D)
	$(COMP) -c $< -o $@

clean:
	rm -rf $(B)

fclean: clean
	rm -rf $(NAME) $(SELECT_NAME) $(POLL_NAME)

re: fclean all

.PHONY: all clean fclean re
