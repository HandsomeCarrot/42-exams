#include "main.h"	//t_data, SUCCESS/FAILURE
#include <stdio.h>	//fscanf, FILE, fprintf, stdout, stdin
#include <stdlib.h>	//EXIT_SUCCES, EXIT_FAILURE
#include <time.h>

/**
 * @return FAILURE/SUCCESS
 */
int isPrint(char c)
{
	if (c >= 33 && c <= 126)
		return (SUCCESS);
	printf("fail: isPrint: '%c'\n", c); //remove
	return (FAILURE);
}

/**
 * @return FAILURE/SUCCESS
 */
int isNum(char c)
{
	if (c >= '0' && c <= '9')
		return (SUCCESS);
	printf("fail: isNum: '%c'\n", c); //remove
	return (FAILURE);
}

/**
 * @return FAILURE/SUCCESS
 */
int getNextChar(char * c, FILE * stream)
{
	if (fscanf(stream, "%c", c) != 1)
	{
		printf("fail: getNextChar\n"); //remove
		return (FAILURE);
	}

	return (SUCCESS);
}

/**
 * @return '-1' if something goes wrong, '>= 0' if successful
 */
int getNumber(char * last_char, FILE * stream)
{
	int result = 0;

	while (getNextChar(last_char, stream) == SUCCESS)
	{
		if (!isNum(*last_char))
			break ;

		result = result * 10 + (int)(*last_char - '0');

		if (result < 0)
		{
			printf("fail: getNumber: overflow\n"); //remove
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
		printf("fail: validFirstLine: no stream\n"); //remove
		return (FAILURE);
	}

	char newline;

	data->map.height = getNumber(&data->tiles.empty, stream);		//get map height and the 'empty' char

	if (data->map.height <= 0													//check valid game height
		|| !isPrint(data->tiles.empty)										//check for valid char (empty)
		|| getNextChar(&data->tiles.obstacle, stream) == FAILURE				//get char (obstacle)
		|| !isPrint(data->tiles.obstacle)									//check for valid char (obstacle)
		|| data->tiles.empty == data->tiles.obstacle							//check for duplicate character ('empty' & 'obstacle')
		|| getNextChar(&data->tiles.full, stream) == FAILURE					//get char (full)
		|| !isPrint(data->tiles.full)										//check for valid char (full)
		|| data->tiles.empty == data->tiles.full								//check for duplicate character ('empty' & 'full')
		|| data->tiles.obstacle == data->tiles.full								//check for duplicate character ('obstacle' & 'full')
		|| getNextChar(&newline, stream) == FAILURE							//get next character
		|| newline != '\n')														//has to be a newline character, otherwise map is invalid
	{
		printf("fail: validFirstLine: invalid format\n"); //remove
		return (FAILURE);
	}

	printf("[INFO] map height: %d | chars(empty,obstacle,full): '%c', '%c', '%c'\n", data->map.height, data->tiles.empty, data->tiles.obstacle, data->tiles.full);

	return (SUCCESS);
}

int main(int argc, char ** argv)
{
	t_data data;

	if (argc == 1)
	{
		if (validFirstLine(stdin, &data) == FAILURE)
		{
			fprintf(stdout, "Error: map invalid\n");
			return (EXIT_FAILURE);
		}
	}

	printf("success\n"); //remove
	return EXIT_SUCCESS;
}