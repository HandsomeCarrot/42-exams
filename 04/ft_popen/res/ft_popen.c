#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>

static bool wrong_input(const char *file, char *const argv[], char type)
{
	if (!file || !*file)
		return (true);
	if (!argv || !*argv)
		return (true);
	if (type != 'r' && type != 'w')
		return (true);
	return (false);
}

static int execute(const char *file, char *const argv[], char type, int fds[2])
{
	pid_t child = fork();
	if (child == -1)
		return (-1);
	if (child > 0)
		return (0);
	if (type == 'r')
		dup2(fds[1], STDOUT_FILENO);
	else
		dup2(fds[0], STDIN_FILENO);
	close(fds[0]);
	close(fds[1]);
	execvp(file, argv);
	exit(-1);
	return (-1);
}

static int get_fd(char type, int fds[2])
{
	if (type == 'r')
	{
		close(fds[1]);
		return (fds[0]);
	}
	close(fds[0]);
	return (fds[1]);
}

int ft_popen(const char *file, char *const argv[], char type)
{
	int	fds[2];

	if (wrong_input(file, argv, type))
		return (-1);
	if (pipe(fds) == -1)
		return (-1);
	if (execute(file, argv, type, fds) == -1)
		return (-1);
	return (get_fd(type, fds));
}
