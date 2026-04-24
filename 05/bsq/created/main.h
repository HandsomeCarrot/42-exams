#ifndef MAIN_H
#define MAIN_H

#define SUCCESS 1
#define FAILURE 0

struct s_tile
{
	char empty;
	char obstacle;
	char full;
};

struct s_map
{
	char ** layout;
	int width;
	int height;
};

struct s_square
{
	int size;
	int x;
	int y;
};

typedef struct s_data
{
	struct s_map map;
	struct s_tile tiles;
	struct s_square square;
} t_data;

#endif