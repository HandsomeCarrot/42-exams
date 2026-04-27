#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>	//FILE, fprintf, stdout, stdin, fopen, fclose
#include <stdlib.h>	//EXIT_SUCCESS, EXIT_FAILURE, NULL, free, calloc

#define SUCCESS 1
#define FAILURE 0

struct s_tile
{
	char empty;
	char obstacle;
	char full;
};

struct s_map
{
	char ** layout;
	int width;
	int height;
};

struct s_square
{
	int size;
	int x;
	int y;
};

typedef struct s_data
{
	struct s_map map;
	struct s_tile tiles;
	struct s_square square;
} t_data;

int isPrint(char c);
int isNum(char c);
int ftStrLen(char * str);
int getNextLine(char ** line, FILE * stream);
int getNumber(char * str, int * index);
int validFirstLine(FILE * stream, t_data * data);
int validMapCharacters(char * line, struct s_tile * tiles);
int validMap(FILE * stream, t_data * data);
int validSquare(int square_size, int x, int y, t_data * data);
int findBSQ(t_data * data);
void insertBSQ(t_data * data);
void printMap(FILE * stream, t_data * data);
void freeMap(t_data * data);
void cleanup(t_data * data, FILE * stream);

#endif