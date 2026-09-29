#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

enum
{
	MAX_FD = 1024,
	MAX_BUFFER_SIZE = 4096,
};

typedef struct client
{
	int		id;
	char	*buf;
}			t_client;

int				g_server_fd, g_next_id, g_largest_fd;
char			g_buffer[MAX_BUFFER_SIZE];
struct pollfd	g_pfds[MAX_FD];
t_client		g_clients[MAX_FD];

int extract_message(char **buf, char **msg)
{
	char *newbuf;
	int i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char *newbuf;
	int len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void cleanup(void)
{
	for (int i = 0; i <= g_largest_fd; ++i)
	{
		if (g_clients[i].buf)
		{
			free(g_clients[i].buf);
			g_clients[i].buf = 0;
		}
		if (g_clients[i].id >= 0)
		{
			close(i);
			g_clients[i].id = -1;
		}
	}
	if (g_server_fd >= 0)
	{
		close(g_server_fd);
		g_server_fd = -1;
	}
}

void fatal_error(void)
{
	cleanup();
	write(STDERR_FILENO, "Fatal error\n", 12);
	exit(1);
}

void init(int port)
{
	for (int i = 0; i < MAX_FD; ++i)
	{
		g_clients[i].id = -1;
		g_pfds[i].fd = -1;
	}

	g_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (g_server_fd == -1)
		fatal_error();

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(port);

	if ((bind(g_server_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0
		|| (listen(g_server_fd, 10)) != 0)
		fatal_error();

	g_largest_fd = g_server_fd;
	g_pfds[g_server_fd].events = POLLIN;
	g_pfds[g_server_fd].fd = g_server_fd;
}

void broadcast_msg(int sender_fd, char *server_msg, char *client_msg)
{
	for (int i = 0; i <= g_largest_fd; ++i)
	{
		if (i == g_server_fd || i == sender_fd
			|| g_clients[i].id == -1 || !(g_pfds[i].revents & POLLOUT))
			continue ;

		if (server_msg)
			send(i, server_msg, strlen(server_msg), MSG_NOSIGNAL);
		if (client_msg)
			send(i, client_msg, strlen(client_msg), MSG_NOSIGNAL);
	}
}

void receive_client(void)
{
	if (!(g_pfds[g_server_fd].revents & POLLIN))
		return ;

	int nfd = accept(g_server_fd, NULL, NULL);
	if (nfd < 0)
		return ; // or fatal_error, depending on subject interpretation
	if (nfd >= MAX_FD)
	{
		close(nfd);
		return ; // same here, could be a fatal_error, depending on subject interpretation
	}

	g_pfds[nfd].fd = nfd;
	g_pfds[nfd].events = POLLIN | POLLOUT;
	g_pfds[nfd].revents = 0;
	g_clients[nfd].id = g_next_id++;
	if (nfd > g_largest_fd)
		g_largest_fd = nfd;

	sprintf(g_buffer, "server: client %d just arrived\n", g_clients[nfd].id);
	broadcast_msg(nfd, g_buffer, NULL);
}

void disconnect_client(int fd)
{
	t_client *client = &g_clients[fd];

	sprintf(g_buffer, "server: client %d just left\n", client->id);
	broadcast_msg(fd, g_buffer, NULL);

	client->id = -1;
	if (client->buf)
	{
		free(client->buf);
		client->buf = 0;
	}
	g_pfds[fd].fd = -1;
	g_pfds[fd].events = 0;
	close(fd);

	if (fd == g_largest_fd)
	{
		int nlfd = fd - 1;
		while (nlfd > g_server_fd && g_clients[nlfd].id == -1)
			--nlfd;
		g_largest_fd = nlfd;
	}
}

int receive_msg(int fd)
{
	if (!(g_pfds[fd].revents & POLLIN))
		return (0);

	int r_size = recv(fd, g_buffer, MAX_BUFFER_SIZE - 1, 0);
	if (r_size <= 0)
	{
		disconnect_client(fd);
		return (0);
	}

	g_buffer[r_size] = '\0';
	char *nbuf = str_join(g_clients[fd].buf, g_buffer);
	if (!nbuf)
		fatal_error();
	g_clients[fd].buf = nbuf;
	return (1);
}

void send_msgs(int fd)
{
	t_client *client = &g_clients[fd];
	char *client_msg = 0;

	while (1)
	{
		int r_size = extract_message(&(client->buf), &client_msg);
		if (r_size == 0)
			return ;
		else if (r_size == -1)
			fatal_error();

		sprintf(g_buffer, "client %d: ", client->id);
		broadcast_msg(fd, g_buffer, client_msg);

		free(client_msg);
	}
}

void route_msgs(void)
{
	for (int i = 0; i <= g_largest_fd; ++i)
	{
		if (i == g_server_fd
			|| receive_msg(i) <= 0)
			continue ;
		send_msgs(i);
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(STDERR_FILENO, "Wrong number of arguments\n", 26);
		return (1);
	}

	init(atoi(argv[1]));

	while (1)
	{
		if (poll(g_pfds, g_largest_fd + 1, 500) <= 0)
			continue ;
		route_msgs();
		receive_client();
	}
	cleanup();
	return (0);
}
