#include "main.h"	//t_data, SUCCESS/FAILURE

/**
 * @brief writes the map layout to @p stream, row by row
 */
void printMap(FILE * stream, t_data * data)
{
	for (int y = 0; y < data->map.height; ++y)
	{
		fprintf(stream, "%s", data->map.layout[y]);
	}
}

/**
 * @brief frees all memory allocated for the map layout
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
 * @brief closes the stream (if not stdin) and frees map memory
 */
void cleanup(t_data * data, FILE * stream)
{
	if (stream != stdin)
		fclose(stream);
	freeMap(data);
}

/**
 * @brief entry point; reads map from file or stdin, solves BSQ, prints result
 *
 * Usage: ./bsq [map_file]
 * If no file is given, reads from stdin.
 *
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
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
