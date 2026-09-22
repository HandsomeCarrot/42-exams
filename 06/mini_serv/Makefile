SELECT_NAME := serv_select
POLL_NAME := serv_poll

CC := cc
CFLAGS := -Wall -Wextra -Werror -g -fsanitize=address,undefined,leak
INCLUDE := -Icreated
COMP := $(CC) $(CFLAGS) $(INCLUDE)

SELECT_SRC := created/mini_serv.c
POLL_SRC := created/mini_serv_poll.c

B := build
SELECT_OBJ := $(SELECT_SRC:%.c=$(B)/%.o)
POLL_OBJ := $(POLL_SRC:%.c=$(B)/%.o)

all: $(SELECT_NAME)

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
	rm -rf $(SELECT_NAME) $(POLL_NAME)

re: fclean all

.PHONY: all clean fclean re
