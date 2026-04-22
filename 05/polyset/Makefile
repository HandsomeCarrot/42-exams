NAME := test

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -g

INCLUDE := -Igiven -Icreated
COMP := $(CXX) $(CXXFLAGS) $(INCLUDE)

SRCS :=	given/main.cpp \
		given/array_bag.cpp \
		given/tree_bag.cpp \
		created/searchable_array_bag.cpp \
		created/searchable_tree_bag.cpp \
		created/set.cpp

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

.PHONY: all clean flcean re