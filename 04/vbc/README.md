# vbc (passed)

## status

passed (2025-09-05)

>tries: 2

## toc

- [subject](#subject)
- [file structure](#file-structure)
- [usage](#usage)
- [personal notes](#personal-notes)

## subject

Assignment name  : vbc

Expected files   : *.c *.h

Allowed functions: malloc, calloc, realloc, free, printf, isdigit, write

Write a program that will print the result of a math expression given as argument.
You must handle the operations `+` `*` and the parenthesis.
You don't have to handle whitespaces in the expression.
All the values in the expression will be between 0 and 9 included.
In case of unexpected symbol or inappropriate parenthesis, you will print `Unexpected token '%c'\n` and exit with the code 1 (if the symbol is the end of input you will print: `Unexpected end of input\n`).
In case of a syscall failure you will just exit with the code 1.

You will be given the beginning of the code (in `vbc.c`), which includes:
- a `node` struct with an enum type (`ADD`, `MULTI`, `VAL`), a `val` field, and `l`/`r` child pointers
- helper functions: `new_node()`, `destroy_tree()`, `unexpected()`, `accept()`, `expect()`, `eval_tree()`
- a skeleton `parse_expr()` function
- a `main()` that parses the argument, evaluates the tree, prints the result, and destroys the tree

You must implement the expression parsing logic.

Examples:
```bash
$> ./vbc '1' | cat -e
1$
$> ./vbc '2+3' | cat -e
5$
$> ./vbc '3*4+5' | cat -e
17$
$> ./vbc '3+4*5' | cat -e
23$
$> ./vbc '(3+4)*5' | cat -e
35$
$> ./vbc '(((((2+2)*2+2)*2+2)*2+2)*2+2)*2' | cat -e
188$
$> ./vbc '1+' | cat -e
Unexpected end of input$
$> ./vbc '1+2)' | cat -e
Unexpected token ')'$
$> ./vbc '((1+3)*12+(3*(2+6))' | cat -e
Unexpected token '2'$
```

## file structure

The subject is in this readme and not in a separate file. All other files are organized as follows:

### solution

My implementation files:
- `include/vbc.h` — header with struct definitions, function prototypes, includes
- `srcs/vbc.c` — main function (entry point)
- `srcs/given.c` — implementations of the given helper functions (`new_node`, `destroy_tree`, `unexpected`, `accept`, `expect`, `eval_tree`)
- `srcs/parsing.c` — my implementation of the recursive descent parser (`parse`, `parse_add`, `parse_multi`, `parse_num`)
- `Makefile` — builds the `vbc` executable

### given

Files you will be given for this exercise:
- `vbc.c` — skeleton code with struct definitions, helper functions, and an empty `parse_expr()`
- `subject.txt` — the subject text

## usage

- compile using `make`, or `make re`

  > this will create the executable file `vbc`
- execute the program with a math expression as argument:
  - `./vbc '<expression>'`

Examples:
```bash
./vbc '2+3'
./vbc '3+4*5'
./vbc '(3+4)*5'
```

## personal notes

nothing to see here :(
>I generated the README with AI, a few months after the actual exam. So no guarantee of precise information. Only the passing date is 100% correct.