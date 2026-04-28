# bsq (reworked)

## status

failed in exam
> reworked afterward

>tries: 1

## toc

- [subject](#subject)
- [file structure](#file-structure)
- [usage](#usage)
- [personal notes](#personal-notes)

## subject

> might slightly vary from actual subject

Assignment name : bsq

Expected files : *.c *.h

Allowed functions and globals: malloc, calloc, realloc, free, fopen, fclose,
getline, fscanf, fputs, fprintf, stderr, stdout, stdin, errno

The aim of this program is to find the biggest square on a map, avoiding obstacles.
A file containing the map will be provided. It'll have to be passed as an argument to your program.
The first line of the map contains information on how to read the map:

- The number of lines on the map
- The "empty" character
- The "obstacle" character
- The "full" character

	The map is made up of '"empty" characters', lines and '"obstacle" characters'.
	The aim of the program is to replace '"empty" characters' by '"full" characters' in order to represent the biggest square possible.
	In the case that more than one solution exists, we'll choose to represent the square that's closest to the top of the map, then the one that's most to the left.

Should there be no passed arguments, your program must be able to read on the standard input.

Definition of a valid map :

- All lines must have the same length.
- There's at least one line of at least one box.
- At each end of line, there's a line break.
- The characters on the map can only be those introduced in the first line.
- The map is invalid if a character is missing from the first line, or if two characters (of empty, full and obstacle) are identical.
- The characters can be any printable character, even numbers.
- In case of an invalid map, your program should display "Error: invalid map" on the standard output followed by a line break.

example:
```bash
%>cat example_file
9.ox
...........................
....o......................
............o..............
...........................
....o......................
...............o...........
...........................
......o..............o.....
..o.......o................
%>./bsq example_file
.....xxxxxxx...............
....oxxxxxxx...............
.....xxxxxxxo..............
.....xxxxxxx...............
....oxxxxxxx...............
.....xxxxxxx...o...........
.....xxxxxxx...............
......o..............o.....
..o.......o................
%>
```

## file structure

The subject is in this readme and not in a separate file. All other files are organized as follows:

### created

Files I created and implemented for this exercise.

### given

Files you will be given for this exercise, but should not be pushed.

### root

All files in here are extra files, that you do not need at all for this exercise.

## usage

Compile the program:

`make` or `make re`

This produces the `test` executable.

### running the program

**With a file argument:**

```bash
./test <map_file>
```

Reads the map from `<map_file>` and prints the solved map to stdout.

Example:
```bash
./test given/example_file
```

**From standard input:**

```bash
./test
```

Then type (or pipe) the map content.

Example:
```bash
./test
3.oX
...
.o.
...
```

### error output

The program prints error messages to stdout, not **stderr**:

| Error                                  | Message                         |
| -------------------------------------- | ------------------------------- |
| Invalid file (cannot open)             | `Error: given file is invalid`  |
| Wrong number of arguments (>1)         | `Error: invalid use of program` |
| Invalid map (bad format or unsolvable) | `Error: map invalid`            |

## personal notes

- I failed this exercise mainly, because I read a lot of manuals for the new functions which we never used before and ran out of time.
- The hardest part of this exercise, in my opinion, is the parsing from different outputs.
	- The first time I was thinking too much about which function to use to get the input (`fscanf` / `getline`).
	- You could probably use `fscanf` for parsing the first line to make it easier (you can parse the number into an int directly), but then you have less control, at least thats what I feel like. I wasn't sure if they will give you wrong inputs,so I just stuck with getline.
- The actual logic, in finding the biggest square, can be pretty easy.
	- iterate over each coordinate, starting at [0,0] and increasing the x-coordinate first.
	> [0,0] -> ... -> [width,0] -> [0, 1] -> ... -> [width, height]
	- trying to find the biggest square possible for that starting point
	> squares => width and height are the same, so size 2 means the square is 2x2. Can not be 2x3.
		- if we found a square before, I will see if a square, bigger by one then the previous bsq, fits there
			- if it fits I continue trying finding a bigger square
			- if it doesn't fit, I move to the next coordinate
	> this method automatically finds the bsq and if there are multiple squares of the same size, it will automatically take the top-left-most one.
	- after it went through all possibilities I just put the 'full'-characters at the squares position
	- then I just draw the completed board.
	
