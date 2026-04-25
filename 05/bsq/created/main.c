#include "main.h"	//t_data, SUCCESS/FAILURE
#include <stdio.h>	//fscanf, FILE, fprintf, stdout, stdin
#include <stdlib.h>	//EXIT_SUCCES, EXIT_FAILURE
#include <string.h>
#include <time.h>

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
	printf("[INFO] strlen(size): %lu(%lu) | '%s'\n", strlen(*line), str_size, *line); //remove
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

	if (getNextLine(&first_line, stream) == FAILURE || ftStrLen(first_line) < 5) //if gnl fails, or first line is smaller than 5 (1 num, 3 chars, 1 newline) characters -> fail
	{
		printf("[FAIL] ValidFirstLine: gnl\n");
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

// int validMap(FILE * stream, t_data * data)
// {
// 	data->map.layout = calloc(data->map.height, sizeof(char *));

// 	for (int i = 0; i < data->map.height; ++i)
// 	{
// 		if (getNextLine(data->map.layout[i], stream) == FAILURE)
// 			return (FAILURE);
// 	} //TODO: validate line lengths, characters used, last line / line after last map line (last line has to be empty)
// 	return (SUCCESS);
// }

int main(int argc, char ** argv)
{
	(void)argv;
	t_data data;

	if (argc == 1)
	{
		if (validFirstLine(stdin, &data) == FAILURE)
			// || validMap(stdin, &data) == FAILURE)
		{
			fprintf(stdout, "Error: map invalid\n");
			return (EXIT_FAILURE);
		}
	}

	printf("[INFO] success\n"); //remove
	return EXIT_SUCCESS;
}