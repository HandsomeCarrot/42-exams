NAME := test

CXX := c++
CXXFLAGS := -g -fsanitize=address,undefined,leak -Wall -Wextra -Werror -std=c++98
INCLUDE := -Icreated -Igiven
COMP := $(CXX) $(CXXFLAGS) $(INCLUDE)

SRCS :=	given/main.cpp \
		created/bigint.cpp

B := build
OBJS := $(SRCS:%.cpp=$(B)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(COMP) $(OBJS) -o $(NAME)

$(B)/%.o: %.cpp
	@mkdir -p $(@D)
	$(COMP) -c $< -o $@

clean:
	rm -rf $(B)

fclean: clean
	rm -rf $(NAME)

re: fclean all

run: re
	./$(NAME)

.PHONY: all clean fclean re run