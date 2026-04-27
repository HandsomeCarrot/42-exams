#include "main.h"	//t_data, SUCCESS/FAILURE

/**
 * @brief checks if @p c is a printable ASCII character (33-126)
 *
 * @return FAILURE/SUCCESS
 */
int isPrint(char c)
{
	if (c >= 33 && c <= 126)
		return (SUCCESS);
	return (FAILURE);
}

/**
 * @brief checks if @p c is a digit ('0'-'9')
 *
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
 * @brief reads a full line from @p stream using getline
 *
 * @param line - output pointer to the non-allocated line buffer
 * @param stream - input stream to read from
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
 * @brief parses a non-negative integer from @p str starting at @p index
 *
 * Advances @p index past the parsed digits. Stops on the first
 * non-digit character (including '\0').
 *
 * @param str - string to parse the number from
 * @param index - on entry: starting position; on exit: position after the last digit
 *
 * @return '-1' if overflow occurs, '>= 0' if successful
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
