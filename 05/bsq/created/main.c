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
	printf("[WARNING] isPrint: '%c'\n", c); //remove
	return (FAILURE);
}

/**
 * @return FAILURE/SUCCESS
 */
int isNum(char c)
{
	if (c >= '0' && c <= '9')
		return (SUCCESS);
	printf("[WARNING] isNum: '%c'\n", c); //remove
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
	int read = 0; //remove
	char * new_line = NULL;
	unsigned long str_size = 0;

	read = getline(&new_line, &str_size, stream);

	printf("[INFO] read: %d | size: %lu\n", read, str_size); //remove
	if (!new_line)
	{
		printf("[FAIL] getNextLine\n"); //remove
		return (FAILURE);
	}

	*line = new_line;
	printf("[INFO] strlen(size): %d(%lu) | '%s'\n", ftStrLen(*line), str_size, *line); //remove
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
		{
			printf("[FAIL] getNumber: overflow\n"); //remove
			return (-1);
		}
	}

	return (result);
}

/**
 * @return FAILURE/SUCCESS
 */
int validFirstLine(FILE * stream, t_data * data)
{
	if (!stream)
	{
		printf("[FAIL] validFirstLine: no stream\n"); //remove
		return (FAILURE);
	}

	char * first_line = NULL;

	if (getNextLine(&first_line, stream) == FAILURE)
	{
		printf("[FAIL] ValidFirstLine: gnl\n");
		return (FAILURE);
	}

	if (ftStrLen(first_line) < 5)												//check line length (1 num, 3 chars, 1 newline), not more/less
	{
		free(first_line);
		printf("[FAIL] ValidFirstLine: too short\n");
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
		printf("[FAIL] validFirstLine: invalid format\n"); //remove
		free(first_line);
		return (FAILURE);
	}

	printf("[INFO] map height: %d | chars(empty,obstacle,full): '%c', '%c', '%c'\n", data->map.height, data->tiles.empty, data->tiles.obstacle, data->tiles.full); //remove
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
	char ** layout = data->map.layout;

	for (int i = 0; i < data->map.height; ++i)
	{
		if (getNextLine(&layout[i], stream) == FAILURE)
		{
			printf("[FAIL] validMap: gnl failed\n"); //remove
			return (FAILURE);
		}

		int current_width = ftStrLen(layout[i]);

		if (layout[i][current_width - 1] != '\n' || current_width <= 1)
		{
			printf("[FAIL] validMap: (does not end with / only has) a newline\n"); //remove
			return (FAILURE);
		}

		if (i == 0)
			data->map.width = current_width - 1;
		else if ((current_width - 1) != data->map.width)
		{
			printf("[FAIL] validMap: line lengths do not match\n"); //remove
			return (FAILURE);
		}

		if (validMapCharacters(layout[i], &data->tiles) == FAILURE)
		{
			printf("[FAIL] validMap: invalid character detected\n"); //remove
			return (FAILURE);
		}
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
			{
				printf("[FAIL] validSquare(%d [%d, %d]): obstacles in the way\n", square_size, x, y); //remove
				return (FAILURE);
			}
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
	{
		printf("[FAIL] findBSQ: no square found\n"); //remove
		return (FAILURE);
	}
	return (SUCCESS);
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

/**
 * TODO: check for minimum map size (at least one line with ?1 'empty' char?) - done
 * TODO: find biggest square - done
 * TODO: modify map to have bsq marked
 * TODO: print map
 */
int main(int argc, char ** argv)
{
	(void)argv;
	t_data data;

	if (argc == 1)
	{
		if (validFirstLine(stdin, &data) == FAILURE
			|| validMap(stdin, &data) == FAILURE
			|| findBSQ(&data) == FAILURE)
		{
			fprintf(stdout, "Error: map invalid\n");
			freeMap(&data);
			return (EXIT_FAILURE);
		}
	}

	printf("[INFO] success\n"); //remove
	freeMap(&data);

	return EXIT_SUCCESS;
}