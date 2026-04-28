# life (passed)

## status

passed (2026-04-28)
> there can be small differences in the code and subject that I did not notice

> tries: 1

## toc

- [subject](#subject)
- [file structure](#file-structure)
- [usage](#usage)
- [personal notes](#personal-notes)

## subject

Assignment name : life

Expected files : *.c *.h

Allowed functions: atoi, read, putchar, malloc, calloc, realloc, free


You must write a program that will simulate a game of life.
The prototype of this program is the following:
`./life <width> <height> <iterations>`
Width and height are the dimensions of the board and iterations is the number of
iterations of the game of life.
The initial configuration of the board will be drawn by a sequence of commands
in the standard input.
Imagine a pen starting in the top left corner of the board.
Each command is one of these characters:

w a s d -> move the pen up, left, bottom, and right.

x -> lift or lower the pen in order to start or stop drawing.


Once end of file is reached, your program must simulate a game of life on this
board and print the result in stdout (each cell alive will be represented by
the character 'O' and each dead cell will be represented by a space ' ').
Each cell outside of the array will be considered dead.

Example:
```bash
$> echo 'sdxddssaaww' | ./a.out 5 5 0 | cat -e
     $
 OOO $
 O O $
 OOO $
     $
$> echo 'sdxssdswdxddddsxaadwxwdxwaa' | ./a.out 10 6 0 | cat -e
          $
 0   OOO  $
 O     O  $
 OOO  O   $
  O  OOO  $
          $
$> echo 'dxss' | ./a.out 3 3 0 | cat -e
 O $
 O $
 O $
$> echo 'dxss' | ./a.out 3 3 1 | cat -e
   $
OOO$
   $
$> echo 'dxss' | ./a.out 3 3 2 | cat -e
 O $
 O $
 O $
```

> - Addition: the rules for the simulation are the same as for "Conway's game of life". I described it in [logic flow](#logic-flow).
> - Addition: the rules for the game of life are given in a additional file in the exam, I just don't remember the details so I do not have it here.

## file structure

The subject is in this readme and not in a separate file. All other files are organized as follows:

### created

Files I created and implemented for this exercise.

### root

All files in here are extra files, that you do not need at all for this exercise.

## usage

- compile using `make`, or `make re`
  > this will create the executable file 'test'
- execute the program `./test <width> <height> <iterations>`
  - width: board width
  - height: board height
  - iterations: number of game of life iterations
- input draw instructions using [pre-defined characters](#subject), or pipe the draw instructions in before execution

Examples:
```bash
./test 3 3 1
# or
echo 'dxss' | ./test 3 3 1
```

## personal notes

### logic flow

- parse the arguments given to the program
	- just used atoi, without any protections, as they do not test that
- get the draw instructions from the standard input
	- used read in a loop, parsing character by character
	- if a character did not match the specified characters in the subject, I just skip it
	- track the pen coordinates and mark them on the board if the pen is "drawing"
	- repeat until read returns an error/eof
- simulate the game of life 'iterations' times
	- create a copy of the board
	- simulate each cell one by one, by counting its neighbors
		- check in a `3x3` matrix around the cell (from the copy, as it is untouched)
		- cells outside of board bounds always count as dead
		- the middle cell (it self) does not count as a neighbor
		- save the outcome (dead/alive) for this cell in the original board
	- delete the copy
	- repeat for 'iterations' amount of times
- print the result
	- print each cell, one by one, using `putchar`
	> don't forget to print new-line characters

### design decisions

- used a 1 dimensional bool array
	- its a bool array, because there are only 2 states for a cell (dead/alive -> false/true)
	- 1 dimension, to make cleanup easier
		- pretty straight forward mapping from 2D-coordinates to a 1D-index
			- `row * (amount of columns in a row) + column`
			> `[1, 5] - (each row has 5 columns)` - translates to `(1 * 5 + 5)` = `10` (the index in the 1D-array, of the 2D-coordinate)
- all other variables in my `t_data` struct are straight forward
- all characters for input parsing and output can be easily modified in the header
