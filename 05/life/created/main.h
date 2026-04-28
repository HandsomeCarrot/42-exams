#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>	// bool

#define ERROR 0
#define SUCCESS 1

#define MOVE_UP 'w'
#define MOVE_LEFT 'a'
#define MOVE_DOWN 's'
#define MOVE_RIGHT 'd'
#define PEN_STANCE 'x'

#define LIFE 'O'
#define DEATH '-'

typedef struct s_data
{
	bool * board;
	int board_width;
	int board_height;
	int game_iterations;
} t_data;

#endif