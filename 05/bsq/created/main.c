#include "main.h"	//t_data, SUCCESS/FAILURE
#include <stdio.h>	//fscanf, FILE, fprintf, stdout, stdin, getline
#include <stdlib.h>	//EXIT_SUCCES, EXIT_FAILURE, NULL, calloc, free

/**
 * @return FAILURE/SUCCESS
 */
int isPrint(char c)
{
	if (c >= 33 && c <= 126)
		return (SUCCESS);
	return (FAILURE);
}

/**
 * @return FAILURE/SUCCESS
 */
int isNum(char c)
{
	if (c >= '0' && c <= '9')
		return (SUCCESS);
	return (FAILURE);
}

/**
 * @brief counts how many characters @p str has
 *
 * @return length of @p str (>= 0)
 */
int ftStrLen(char * str)
{
	int len = 0;

	while (str && str[len])
		++len;

	return (len);
}

/**
 * @brief read a full line from @p stream
 *
 * Uses getline to read a full line from stream.
 *
 * @return SUCCESS/FAILURE
 */
int getNextLine(char ** line, FILE * stream)
{
	char * new_line = NULL;
	unsigned long str_size = 0;

	getline(&new_line, &str_size, stream);

	if (!new_line)
		return (FAILURE);

	*line = new_line;
	return (SUCCESS);
}


/**
 * @param str - str to convert the number from
 * @param index - starting index of the search, but also points
 *                to the character after the last number.
 *
 * @return '-1' if something goes wrong, '>= 0' if successful
 */
int getNumber(char * str, int * index)
{
	int result = 0;

	for (; str[*index]; ++(*index))
	{
		if (!isNum(str[*index]))
			break ;

		result = result * 10 + (int)(str[*index] - '0');

		if (result < 0)
			return (-1);
	}

	return (result);
}

/**
 * @return FAILURE/SUCCESS
 */
int validFirstLine(FILE * stream, t_data * data)
{
	if (!stream)
		return (FAILURE);

	char * first_line = NULL;

	if (getNextLine(&first_line, stream) == FAILURE)
		return (FAILURE);

	if (ftStrLen(first_line) < 5)												//check line length (1 num, 3 chars, 1 newline), not more/less
	{
		free(first_line);
		return (FAILURE);
	}

	int str_index = 0;
	data->map.height = getNumber(first_line, &str_index);						//get map height
	data->tiles.empty = first_line[str_index++];								//get 'empty' char
	data->tiles.obstacle = first_line[str_index++];								//get 'obstacle' char
	data->tiles.full = first_line[str_index++];									//get 'full' char

	if (data->map.height <= 0													//check valid game height
		|| !isPrint(data->tiles.empty)											//check for valid char (empty)
		|| !isPrint(data->tiles.obstacle)										//check for valid char (obstacle)
		|| !isPrint(data->tiles.full)											//check for valid char (full)
		|| data->tiles.empty == data->tiles.obstacle							//check for duplicate character ('empty' & 'obstacle')
		|| data->tiles.empty == data->tiles.full								//check for duplicate character ('empty' & 'full')
		|| data->tiles.obstacle == data->tiles.full								//check for duplicate character ('obstacle' & 'full')
		|| first_line[str_index] != '\n')										//has to be a newline character, otherwise map is invalid
	{
		free(first_line);
		return (FAILURE);
	}

	free(first_line);

	return (SUCCESS);
}

/**
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

/**
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
 * @return SUCCESS/FAILURE
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
 * @brief inserts biggest square into map
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

/**
 * @brief prints the map to @p stream
 */
void printMap(FILE * stream, t_data * data)
{
	for (int y = 0; y < data->map.height; ++y)
	{
		fprintf(stream, "%s", data->map.layout[y]);
	}
}

/**
 * @brief frees map
 */
void freeMap(t_data * data)
{
	if (data->map.layout)
	{
		for (int i = 0; i < data->map.height; ++i)
		{
			if (data->map.layout[i])
				free(data->map.layout[i]);
		}
		free(data->map.layout);
	}
}

void cleanup(t_data * data, FILE * stream)
{
	if (stream != stdin)
		fclose(stream);
	freeMap(data);
}

/**
 * TODO: read map from file
 */
int main(int argc, char ** argv)
{
	FILE * stream = stdin;

	if (argc == 2)
	{
		stream = fopen(argv[1], "r");
		if (stream == NULL)
		{
			fprintf(stdout, "Error: given file is invalid\n");
			return (EXIT_FAILURE);
		}
	}
	else if (argc > 2)
	{
		fprintf(stdout, "Error: invalid use of program\n");
		return (EXIT_FAILURE);
	}

	t_data data;
	data.map.layout = NULL;
	data.map.height = 0;
	data.map.width = 0;

	if (validFirstLine(stream, &data) == FAILURE
			|| validMap(stream, &data) == FAILURE
			|| findBSQ(&data) == FAILURE)
		{
			fprintf(stdout, "Error: map invalid\n");
			cleanup(&data, stream);
			return (EXIT_FAILURE);
		}

	insertBSQ(&data);
	printMap(stdout, &data);

	cleanup(&data, stream);

	return EXIT_SUCCESS;
}