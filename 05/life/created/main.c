#include "main.h"	// t_data, bool
#include <stdlib.h>	// EXIT_(FAILURE/SUCCESS), atoi
#include <unistd.h>	// read
#include <stdio.h>	// putchar

/**
 * @brief - parses the three program arguments into the data struct
 *
 * Converts argv[1] (board width), argv[2] (board height), and argv[3]
 * (game iterations) from strings to integers using atoi. Performs no
 * validation — the caller must ensure arguments are present and valid.
 *
 * @param argv	- argument vector from main (indices 1-3 are consumed)
 * @param data	- pointer to the game data struct to populate
 */
void parseArgs(char ** argv, t_data * data)
{
	data->board_width = atoi(argv[1]);
	data->board_height = atoi(argv[2]);
	data->game_iterations = atoi(argv[3]);
}

/**
 * @brief - computes the linear index for a 2D board coordinate
 *
 * Maps the 2D position (x, y) into a 1D array index using row-major ordering.
 * The board is stored as a flat array of size board_width * board_height.
 *
 * @param x		- column coordinate (0-indexed)
 * @param y		- row coordinate (0-indexed)
 * @param data	- pointer to the game data struct containing board dimensions
 *
 * @return the linear index into the board array
 */
int getBoardIndex(int x, int y, t_data * data)
{
	return (y * data->board_width + x);
}

/**
 * @brief - reads the initial board configuration from stdin
 *
 * Interprets a sequence of movement commands (w/a/s/d) read from stdin as a
 * virtual pen that draws live cells onto the board. The pen starts at (0, 0)
 * in the "up" (non-drawing) state. Pressing 'x' toggles drawing on/off.
 * Movements that go out of bounds are silently ignored when drawing.
 *
 * Accepted commands:
 *   w - move pen up one row
 *   a - move pen left one column
 *   s - move pen down one row
 *   d - move pen right one column
 *   x - toggle pen stance (drawing on/off)
 *
 * @param data	- pointer to the game data struct (board must already be allocated)
 */
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

/**
 * @brief - creates a deep copy of the current board state
 *
 * Allocates a new boolean array of the same dimensions as the current board
 * and copies every cell value into it. The caller is responsible for freeing
 * the returned copy.
 *
 * @param data	- pointer to the game data struct
 *
 * @return pointer to the newly allocated board copy, or NULL on allocation failure
 */
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

/**
 * @brief - computes the next state of a single cell using Conway's Game of Life
 *
 * Counts the number of live neighbors (8-directional) surrounding the cell at
 * (base_x, base_y) on the old_board. The new state is determined by:
 *   - 2 neighbors: cell retains its current state
 *   - 3 neighbors: cell becomes alive (birth)
 *   - otherwise:   cell dies (under-population or over-population)
 *
 * @param base_x	- column of the cell to evaluate
 * @param base_y	- row of the cell to evaluate
 * @param old_board	- the board state before this iteration
 * @param data		- pointer to the game data struct
 *
 * @return true if the cell should be alive in the next generation, false otherwise
 */
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

/**
 * @brief - runs the Game of Life simulation for the configured number of iterations
 *
 * For each iteration, creates a copy of the board, computes the next state of
 * every cell simultaneously (using the copy as the reference), stores the
 * result back into the main board, and frees the copy.
 *
 * @param data	- pointer to the game data struct
 *
 * @return SUCCESS on completion, or ERROR if a board copy allocation fails
 */
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

/**
 * @brief - renders the current board state to stdout
 *
 * Iterates over every cell in the board and prints the LIFE character ('O')
 * for live cells and the DEATH character (' ') for dead cells. Each row is
 * terminated by a newline.
 *
 * @param data	- pointer to the game data struct
 */
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
 * @brief - entry point of the Game of Life program
 *
 * Validates that exactly 3 arguments are provided, parses them, allocates
 * the board, reads the initial configuration from stdin, runs the simulation
 * for the requested number of iterations, and draws the final board state.
 * All allocated resources are freed before exit.
 *
 * @param argc	- number of command-line arguments
 * @param argv	- array of command-line argument strings
 *                  - argv[0]: program name
 *                  - argv[1]: board width
 *                  - argv[2]: board height
 *                  - argv[3]: number of simulation iterations
 *
 * @return EXIT_SUCCESS on successful completion
 * @return EXIT_FAILURE if argc != 4, board allocation fails,
 *         or simulation encounters an allocation error
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
