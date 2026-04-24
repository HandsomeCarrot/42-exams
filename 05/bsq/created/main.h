#ifndef MAIN_H
#define MAIN_H

typedef enum e_tile
{
	EMPTY_TILE,
	OBSTACLE_TILE,
	FULL_TILE
} t_tile;

typedef struct s_map
{
	char ** layout;
	char * tiles;
	int width;
	int height;
} t_map;

typedef struct s_square
{
	int size;
	int x;
	int y;
} t_square;

typedef struct s_data
{
	t_map map;
	t_square square;
} t_data;

#endif