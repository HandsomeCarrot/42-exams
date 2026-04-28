#include "main.h"	// t_data, bool
#include <stdlib.h>	// EXIT_(FAILURE/SUCCESS), atoi
#include <unistd.h>	// read
#include <stdio.h>	// putchar

/**
 * @brief - converts program arguments
 *
 * Converts program arguments into integers and saves them in the data struct.
 */
void parseArgs(char ** argv, t_data * data)
{
	data->board_width = atoi(argv[1]);
	data->board_height = atoi(argv[2]);
	data->game_iterations = atoi(argv[3]);
}

int getBoardIndex(int x, int y, t_data * data)
{
	return (y * data->board_width + x);
}

void parseBoardConfig(t_data * data)
{
	char move = 0;
	bool draw = false;
	int pen_x = 0;
	int pen_y = 0;

	while (read(STDIN_FILENO, &move, sizeof(char)) == 1)
	{
		pen_x += (move == MOVE_RIGHT) - (move == MOVE_LEFT);
		pen_y += (move == MOVE_DOWN) - (move == MOVE_UP);

		if (move == PEN_STANCE)
			draw = !draw;

		if (draw == true
			&& pen_x < data->board_width && pen_x >= 0
			&& pen_y < data->board_height && pen_y >= 0)
			data->board[getBoardIndex(pen_x, pen_y, data)] = true;
	}
}

bool * copyBoard(t_data * data)
{
	bool * copy = calloc(data->board_height * data->board_width, sizeof(bool));
	if (!copy)
		return (NULL);

	for (int y = 0; y < data->board_height; ++y)
	{
		for (int x = 0; x < data->board_width; ++x)
			copy[getBoardIndex(x, y, data)] = data->board[getBoardIndex(x, y, data)];
	}

	return (copy);
}

bool getNewState(int base_x, int base_y, bool * old_board, t_data * data)
{
	int neighbor_count = 0;

	for (int y = base_y - 1; y < base_y + 2; ++y)
	{
		if (y < 0 || + y >= data->board_height)
			continue ;

		for (int x = base_x - 1; x < base_x + 2; ++x)
		{
			if (x < 0 || x >= data->board_width
				|| (x == base_x && y == base_y))
				continue ;

			neighbor_count += old_board[getBoardIndex(x, y, data)];
		}
	}

	if (neighbor_count == 2)
		return (old_board[getBoardIndex(base_x, base_y, data)]);
	else if (neighbor_count == 3)
		return (true);
	else
		return (false);
}

int simulateLife(t_data * data)
{
	bool * board_copy = NULL;

	for (int i = 0; i < data->game_iterations; ++i)
	{
		board_copy = copyBoard(data);
		if (!board_copy)
			return (ERROR);

		for (int y = 0; y < data->board_height; ++y)
		{
			for (int x = 0; x < data->board_width; ++x)
				data->board[getBoardIndex(x, y, data)] = getNewState(x, y, board_copy, data);
		}

		free(board_copy);
	}

	return (SUCCESS);
}

void drawBoard(t_data * data)
{
	for (int y = 0; y < data->board_height; ++y)
	{
		for (int x = 0; x < data->board_width; ++x)
		{
			if (data->board[(y * data->board_width) + x])
				putchar(LIFE);
			else
				putchar(DEATH);
		}
		putchar('\n');
	}
}

/**
 * @brief starting point of 'life'
 *
 * @param argc	- amount of arguments
 * @param argv	- arguments passed to the program
 *					- index: 0 -> program name
 *					- index: 1 -> board width
 *					- index: 2 -> board height
 *					- index: 3 -> life iterations
 *
 * @return EXIT_FAILURE / EXIT_SUCCESS
 */
int main(int argc, char ** argv)
{
	if (argc != 4)
		return (EXIT_FAILURE);

	t_data data;

	parseArgs(argv, &data);

	data.board = calloc(data.board_height * data.board_width, sizeof(bool));
	if (!data.board)
		return (EXIT_FAILURE);

	parseBoardConfig(&data);

	if (simulateLife(&data) == ERROR)
	{
		free(data.board);
		return (EXIT_FAILURE);
	}

	drawBoard(&data);

	free(data.board);

	return (EXIT_SUCCESS);
}