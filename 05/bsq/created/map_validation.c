#include "main.h"	//t_data, SUCCESS/FAILURE

/**
 * @brief validates the first line of the map file
 *
 * Expects format: "<height><empty><obstacle><full>\n".
 * Validates height > 0, all three tile characters are printable
 * and distinct from each other, and the line ends with '\n'.
 *
 * @param stream - input stream positioned at the first line
 * @param data - parsed tile characters and map height stored here
 *
 * @return FAILURE/SUCCESS
 */
int validFirstLine(FILE * stream, t_data * data)
{
	if (!stream)
		return (FAILURE);

	char * first_line = NULL;

	if (getNextLine(&first_line, stream) == FAILURE)
		return (FAILURE);

	if (ftStrLen(first_line) < 5)
	{
		free(first_line);
		return (FAILURE);
	}

	int str_index = 0;
	data->map.height = getNumber(first_line, &str_index);
	data->tiles.empty = first_line[str_index++];
	data->tiles.obstacle = first_line[str_index++];
	data->tiles.full = first_line[str_index++];

	if (data->map.height <= 0
		|| !isPrint(data->tiles.empty)
		|| !isPrint(data->tiles.obstacle)
		|| !isPrint(data->tiles.full)
		|| data->tiles.empty == data->tiles.obstacle
		|| data->tiles.empty == data->tiles.full
		|| data->tiles.obstacle == data->tiles.full
		|| first_line[str_index] != '\n')
	{
		free(first_line);
		return (FAILURE);
	}

	free(first_line);

	return (SUCCESS);
}

/**
 * @brief checks that every character in @p line (except trailing '\n') is
 *        either the empty or obstacle tile character
 *
 * @param line - a single map row (including trailing '\n')
 * @param tiles - the tile set to validate against
 *
 * @return SUCCESS/FAILURE
 */
int validMapCharacters(char * line, struct s_tile * tiles)
{
	for (int i = 0; line[i + 1]; ++i)
	{
		if (line[i] != tiles->empty && line[i] != tiles->obstacle)
			return (FAILURE);
	}
	return (SUCCESS);
}

/**
 * @brief reads and validates the full map from @p stream
 *
 * Allocates the map layout, reads each row, checks that every row
 * has the same width and contains only valid tile characters.
 *
 * @param stream - input stream positioned after the first line
 * @param data - map dimensions and tile set; allocated layout stored here
 *
 * @return SUCCESS/FAILURE
 */
int validMap(FILE * stream, t_data * data)
{
	data->map.layout = calloc(data->map.height, sizeof(char *));
	if (!data->map.layout)
		return (FAILURE);
	char ** layout = data->map.layout;

	for (int i = 0; i < data->map.height; ++i)
	{
		if (getNextLine(&layout[i], stream) == FAILURE)
			return (FAILURE);

		int current_width = ftStrLen(layout[i]);

		if (layout[i][current_width - 1] != '\n' || current_width <= 1)
			return (FAILURE);

		if (i == 0)
			data->map.width = current_width - 1;
		else if ((current_width - 1) != data->map.width)
			return (FAILURE);

		if (validMapCharacters(layout[i], &data->tiles) == FAILURE)
			return (FAILURE);
	}
	return (SUCCESS);
}
