# bigint (wip)
## toc
- [subject](#subject)
- [file structure](#file-structure)
- [usage](#usage)
## subject
Assignment name : bigint

Expected files : bigint.hpp, bigint.cpp


In computer science a bignum is an object representing an arbitrary precision
number, this is useful when you want to store a number bigger than SIZE_MAX
without any loss of precision. This is often achieved by storing an array or a
string containing the different "parts" of the number.
Create a class called bigint that will store an arbitrary precision unsigned
integer.

Your class must support addition, comparison and "digitshift" (like bitshift but
instead of shifting the bits you will shift the digits in base 10, e.g.:
(42 << 3 == 42000) and (1337 >> 2 == 13)).
Your bigint must be printable with the << operator (in base 10) and the output
should not contain any leading zeros.
You will find a main in this directory that must work with your class
## file structure
The subject is in this readme. All other files are organized as follows:
### created
Files you need to create, implement and push for this exercise.
### given
Files you will be given for this exercise. Your files will depend on these. The main is a test if your implementation has the wanted behaviour.
> This exercise subject and files are from another public git repo. I did not have this exercise in the exam, so I can not verify the correctness.
### root
All files in here are extra files, that you do not need at all for this exercise. I created the `Makefile` for eaasier testing.
## usage
- compile using `make`, or `make re` - this will create the executable file 'test'
- execute the program `./test <arguments>`
	- arguments: each argument is a number that will be inserted
	- example execution: `./test 1 2 3 4`, `./test 12 34 45`, ...