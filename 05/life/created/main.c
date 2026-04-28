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

void parseBoardConfig(t_data * data)
{
	char move = 0;
	bool draw = false;
	int pen_x = 0;
	int pen_y = 0;

	while (1)
	{
		if (read(STDIN_FILENO, &move, 1) != 1)
			break ;

		pen_x += (move == MOVE_RIGHT) - (move == MOVE_LEFT);
		pen_y += (move == MOVE_DOWN) - (move == MOVE_UP);

		if (move == PEN_STANCE)
			draw = !draw;

		if (draw == true
			&& pen_x < data->board_width && pen_x >= 0
			&& pen_y < data->board_height && pen_y >= 0)
			data->board[pen_y * data->board_width + pen_x] = true;
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
			copy[y * data->board_width + x] = data->board[y * data->board_width + x];
	}

	return (copy);
}

bool getNextGen(int base_x, int base_y, bool * old_board, t_data * data)
{
	int neighbor_count = 0;

	for (int y = -1; y < 2; ++y)
	{
		if ((base_y + y) < 0 || (base_y + y) >= data->board_height)
			continue ;
		for (int x = -1; x < 2; ++x)
		{
			if ((base_x + x) < 0 || (base_x + x) >= data->board_width
				|| (x == 0 && y == 0))
				continue ;

			if (old_board[(base_y + y) * data->board_width + (base_x + x)])
				++neighbor_count;
		}
	}

	if (neighbor_count == 2)
		return (old_board[base_y * data->board_width + base_x]);
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
				data->board[y * data->board_width + x] = getNextGen(x, y, board_copy, data);
		}

		if (board_copy)
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