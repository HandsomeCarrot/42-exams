# ft_popen (passed)

## status

passed (2025-09-02)

>tries: 2

## toc

- [subject](#subject)
- [file structure](#file-structure)
- [usage](#usage)
- [personal notes](#personal-notes)

## subject

Assignment name  : ft_popen

Expected files   : ft_popen.c

Allowed functions: pipe, fork, dup2, execvp, close, exit

Write the following function:

```
int ft_popen(const char *file, char *const argv[], char type);
```

The function must launch the executable file with the arguments argv (using execvp).
If type is 'r' the function must return a file descriptor connected to the output of the command.
If type is 'w' the function must return a file descriptor connected to the input of the command.
In case of error or invalid parameter the function must return -1.

For example, the function could be used like that:

```c
int main()
{
    int  fd;
    char *line;

    fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
    while ((line = get_next_line(fd)))
        ft_putstr(line);
    return (0);
}

int	main() {
	int	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	dup2(fd, 0);
	fd = ft_popen("grep", (char *const []){"grep", "c", NULL}, 'r');
	char	*line;
	while ((line = get_next_line(fd)))
		printf("%s", line);
}
```

Hints:
- Do not leak file descriptors!
- This exercise is inspired by the libc's `popen()`.

## file structure

The subject is in this readme and not in a separate file. All other files are organized as follows:

### res

My implementation file (`ft_popen.c`). This is the file you need to push.

### given

Files you will be given for this exercise:
- `subject.txt` — the subject text

### test

Extra test files I created for local testing:
- `test_main.c` — a test harness with multiple test cases (basic read, read with args, error cases, write functionality)

## usage

- include `ft_popen.c` in your project alongside your own get_next_line implementation
- compile with your test code, e.g.:
  - `cc -Wall -Wextra -Werror test/test_main.c res/ft_popen.c -o test`
- execute the test program:
  - `./test`

## personal notes

nothing to see here :(
>I generated the README with AI, a few months after the actual exam. So no guarantee of precise information. Only the passing date is 100% correct.