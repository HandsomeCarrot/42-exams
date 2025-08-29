/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_main.c                                        :+:      :+:    :+:   */
/*                                                     +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 18:04:00 by vpoka             #+#    #+#             */
/*   Updated: 2025/08/29 18:04:00 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

// Function prototype for ft_popen
int	ft_popen(const char *file, char *const argv[], char type);

// Simple get_next_line implementation for testing
char	*get_next_line(int fd)
{
	static char	buffer[1024];
	static int	pos = 0;
	static int	bytes_read = 0;
	char		*line;
	int			i;

	if (bytes_read == 0 || pos >= bytes_read)
	{
		bytes_read = read(fd, buffer, sizeof(buffer) - 1);
		if (bytes_read <= 0)
			return (NULL);
		pos = 0;
		buffer[bytes_read] = '\0';
	}

	i = 0;
	while (pos + i < bytes_read && buffer[pos + i] != '\n' && buffer[pos + i] != '\0')
		i++;

	line = malloc(i + 2);
	if (!line)
		return (NULL);

	strncpy(line, &buffer[pos], i);
	if (pos + i < bytes_read && buffer[pos + i] == '\n')
	{
		line[i] = '\n';
		line[i + 1] = '\0';
		pos += i + 1;
	}
	else
	{
		line[i] = '\0';
		pos += i;
	}

	return (line);
}

// Test 1: Basic read functionality - list files
void	test_basic_read(void)
{
	printf("=== Test 1: Basic read functionality (ls) ===\n");
	
	int		fd;
	char	*line;
	int		count = 0;

	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	if (fd == -1)
	{
		printf("ERROR: ft_popen failed for ls command\n");
		return;
	}

	printf("Output from 'ls' command:\n");
	while ((line = get_next_line(fd)))
	{
		printf("%s", line);
		free(line);
		count++;
	}

	close(fd);
	printf("Read %d lines from ls command\n\n", count);
}

// Test 2: Read with arguments - find specific files
void	test_read_with_args(void)
{
	printf("=== Test 2: Read with arguments (find . -name \"*.c\") ===\n");
	
	int		fd;
	char	*line;
	int		count = 0;

	fd = ft_popen("find", (char *const []){"find", ".", "-name", "*.c", NULL}, 'r');
	if (fd == -1)
	{
		printf("ERROR: ft_popen failed for find command\n");
		return;
	}

	printf("C files found:\n");
	while ((line = get_next_line(fd)))
	{
		printf("%s", line);
		free(line);
		count++;
	}

	close(fd);
	printf("Found %d C files\n\n", count);
}

// Test 3: Error cases - invalid parameters
void	test_error_cases(void)
{
	printf("=== Test 3: Error cases ===\n");
	
	int	fd;

	// Test NULL file
	fd = ft_popen(NULL, (char *const []){"ls", NULL}, 'r');
	printf("NULL file test: %s (expected: -1)\n", fd == -1 ? "PASS" : "FAIL");

	// Test empty file
	fd = ft_popen("", (char *const []){"ls", NULL}, 'r');
	printf("Empty file test: %s (expected: -1)\n", fd == -1 ? "PASS" : "FAIL");

	// Test NULL argv
	fd = ft_popen("ls", NULL, 'r');
	printf("NULL argv test: %s (expected: -1)\n", fd == -1 ? "PASS" : "FAIL");

	// Test empty argv
	fd = ft_popen("ls", (char *const []){NULL}, 'r');
	printf("Empty argv test: %s (expected: -1)\n", fd == -1 ? "PASS" : "FAIL");

	// Test invalid type
	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'x');
	printf("Invalid type test: %s (expected: -1)\n", fd == -1 ? "PASS" : "FAIL");

	printf("\n");
}

// Test 4: Write functionality - test with cat command
void	test_write_functionality(void)
{
	printf("=== Test 4: Write functionality (cat) ===\n");
	
	int		fd;
	pid_t	pid;
	int		pipe_fds[2];
	char	buffer[1024];
	ssize_t	bytes;

	// Create a pipe to capture output
	if (pipe(pipe_fds) == -1)
	{
		printf("ERROR: Failed to create pipe\n");
		return;
	}

	pid = fork();
	if (pid == -1)
	{
		printf("ERROR: Fork failed\n");
		return;
	}

	if (pid == 0)
	{
		// Child process: use ft_popen to write to cat, then read from pipe
		close(pipe_fds[0]); // Close read end
		
		fd = ft_popen("cat", (char *const []){"cat", NULL}, 'w');
		if (fd == -1)
		{
			printf("ERROR: ft_popen failed for cat command\n");
			exit(1);
		}

		// Write some text to cat
		write(fd, "Hello from ft_popen!\n", 21);
		write(fd, "This is a test message.\n", 24);
		close(fd);
		exit(0);
	}
	else
	{
		// Parent process: read from pipe
		close(pipe_fds[1]); // Close write end
		
		printf("Output from cat command:\n");
		while ((bytes = read(pipe_fds[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytes] = '\0';
			printf("%s", buffer);
		}
		
		close(pipe_fds[0]);
		wait(NULL);
	}

	printf("\n");
}

int	main(void)
{
	printf("Testing ft_popen implementation\n");
	printf("===============================\n\n");

	test_basic_read();
	test_read_with_args();
	test_error_cases();
	test_write_functionality();

	printf("All tests completed.\n");
	return (0);
}
