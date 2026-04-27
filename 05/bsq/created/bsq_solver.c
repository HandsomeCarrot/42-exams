#include "main.h"	//t_data, SUCCESS/FAILURE

/**
 * @brief checks whether a square of @p square_size whose top-left
 *        corner is at (@p x, @p y) fits entirely within the map
 *        and consists only of empty tiles
 *
 * @return SUCCESS/FAILURE
 */
int validSquare(int square_size, int x, int y, t_data * data)
{
	for (int sq_y = 0; sq_y < square_size; ++sq_y)
	{
		for (int sq_x = 0; sq_x < square_size; ++sq_x)
		{
			if (sq_x + x >= data->map.width || sq_y + y >= data->map.height
				|| data->map.layout[sq_y + y][sq_x + x] != data->tiles.empty)
				return (FAILURE);
		}
	}
	return (SUCCESS);
}

/**
 * @brief finds the largest square of empty tiles in the map
 *
 * Iterates over every cell and expands the square as long as
 * validSquare succeeds. The result (size and top-left corner)
 * is stored in data->square.
 *
 * @return FAILURE if no empty tile exists at all, SUCCESS otherwise
 */
int findBSQ(t_data * data)
{
	data->square.size = 0;
	data->square.x = -1;
	data->square.y = -1;

	for (int y = 0; y < data->map.height; ++y)
	{
		for (int x = 0; x < data->map.width; ++x)
		{
			for (int square_size = data->square.size + 1; validSquare(square_size, x, y, data); ++square_size)
			{
				data->square.size = square_size;
				data->square.x = x;
				data->square.y = y;
			}
		}
	}

	if (data->square.size == 0)
		return (FAILURE);
	return (SUCCESS);
}

/**
 * @brief fills the found largest square with the 'full' tile character
 */
void insertBSQ(t_data * data)
{
	for (int y = 0; y < data->square.size; ++y)
	{
		for (int x = 0; x < data->square.size; ++x)
		{
			data->map.layout[data->square.y + y][data->square.x + x] = data->tiles.full;
		}
	}
}
