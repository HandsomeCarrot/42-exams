#include "main.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// int ftStrlen(char * str)
// {
// 	if (!str || !*str)
// 		return (0);

// 	int count = 0;
// 	while (str[count])
// 		++count;

// 	return (count);
// }

// int validFirstLine(t_data * data)
// {
// 	if (data->map.height < 1)
// 		return (0);

// 	char * tiles = data->map.tiles;
// 	if (ftStrlen(tiles) != 3
// 		|| tiles[EMPTY_TILE] == tiles[OBSTACLE_TILE]
// 		|| tiles[EMPTY_TILE] == tiles[FULL_TILE]
// 		|| tiles[OBSTACLE_TILE] == tiles[FULL_TILE])
// 		return (0);

// 	return (1);
// }

// int countDigits(int i)
// {
// 	int digits = 1;
// 	for (; i > 9; i /= 10)
// 		++digits;
// 	return (digits);
// }

// char * getNextLine(void)
// {
// 	char * line = NULL;
// 	size_t line_len = 0;
// 	ssize_t read_chars = 0;

// 	if ((read_chars = getline(&line, &line_len, stdin)) == -1)
// 	{
// 		if (line)
// 			free(line);
// 		return (NULL);
// 	}

// 	return (line);
// }

// int main(int argc, char ** argv)
// {
// 	/*if (argc == 2)
// 	{
// 		t_data data;
// 		data.map.tiles = argv[1];
// 		data.map.height = 1;

// 		if (!valid_first_line(&data))
// 			fprintf(stdout, "Error: invalid map\n");
// 	}*/

// 	// int i;
// 	// char a;
// 	// char b;
// 	// char c;
// 	// char d;
// 	// int read_chars;

// 	// int ret = fscanf(stdin, "%d%c%c%c%c%n", &i, &a, &b, &c, &d, &read_chars);
// 	// int expected_char_count = 4 + countDigits(i);

// 	// if (ret != 5 || d != '\n' || expected_char_count != read_chars || i <= 0) //TODO: also need to check, here or later, if characters are printable (no whitespace characters and so on).
// 	// {
// 	// 	printf("ret: %d | read count: %d | calc count: %d\n", ret, read_chars, expected_char_count);
// 	// 	fprintf(stdout, "Error: invalid map\n");
// 	// 	return 1;
// 	// }

// 	// printf("ret: %d | int(digits): %d(%d) | '%c', '%c', '%c' | count: %u\n", ret, i, countDigits(i), a, b, c, read_chars);

// 	int iters = 0;

// 	while(1)
// 	{
// 		++iters;
// 		printf("iter: %d\n", iters);

// 		char * line = getNextLine();

// 		if (!line)
// 		{
// 			printf("Error: invalid map\n");
// 			return (EXIT_FAILURE);
// 		}

// 		if (ftStrlen(line) == )

// 		/* --- CLEANUP --- */

// 		free(line);
// 	}

// 	return (EXIT_SUCCESS);
// }

int main(int argc, char ** argv)
{
	return EXIT_SUCCESS;
}