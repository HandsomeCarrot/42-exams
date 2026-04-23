# bsq (wip)

## toc

- [subject](#subject)
- [file structure](#file-structure)
- [usage](#usage)

## subject

> might slightly vary from actual subject

Assignment name : bsq

Expected files : _.c _.h

Allowed functions and globals: malloc, calloc, realloc, free, fopen, fclose,
getline, fscanf, fputs, fprintf, stderr, stdout, stdin, errno

The aim of this program is to find the biggest square on a map, avoiding obstacles.
A file containing the map will be provided. It'll have to be passed as an argument for your program.
The first line of the map contains information on how to read the map (space separated) :

- The number of lines on the map;
- The "empty" character;
- The "obstacle" character;
- The "full" character.
  The map is made up of '"empty" characters', lines and '"obstacle" characters'.
  The aim of the program is to replace '"empty" characters' by '"full" characters' in order to represent the biggest square possible.
  In the case that more than one solution exists, we'll choose to represent the square that's closest to the top of the map, then the one that's most to the left.

When your program receives more than one map in argument, each solution or "map error" must be followed by a line break.
Should there be no passed arguments, your program must be able to read on the standard input.

Definition of a valid map :

- All lines must have the same length.
- There's at least one line of at least one box.
- At each end of line, there's a line break.
- The characters on the map can only be those introduced in the first line.
- The map is invalid if a character is missing from the first line, or if two characters (of empty, full and obstacle) are identical.
- The characters can be any printable character, even numbers.
- In case of an invalid map, your program should display "map error" on the error output followed by a line break. Your program will then move on to the next map.

example:
%>cat example_file
9 . o x
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

## file structure

The subject is in this readme and not in a seperate file. All other files are organized as follows:

### created

Files you need to create, implement and push for this exercise.

### given

Files you will be given for this exercise, but should not be pushed.

### root

All files in here are extra files, that you do not need at all for this exercise.

## usage
