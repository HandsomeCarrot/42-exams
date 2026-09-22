#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>

enum constants
{
	MAX_CLIENTS = 1024,
	CLIENT_BUFFER_SIZE = 1024,
	MAX_SERVER_MSG_LEN = 40,
	SERVER_BUFFER_SIZE = (CLIENT_BUFFER_SIZE + MAX_SERVER_MSG_LEN),
	CONNECTION_QUEUE_SIZE = 10
}	;

typedef struct client
{
	int		id;
	int		connected;
	int		buf_used;
	char	buf[CLIENT_BUFFER_SIZE];
}			t_client;

typedef struct server
{
	int				fd;
	int				largest_fd;
	int				next_id;
	struct pollfd	poll_fds[MAX_CLIENTS];
	char			buf[SERVER_BUFFER_SIZE];
}					t_server;

t_server	g_server;
t_client	g_clients[MAX_CLIENTS];

void exit_fatal(void)
{
	write(STDERR_FILENO, "Fatal error\n", 12);

	if (g_server.fd >= 0)
		close(g_server.fd);

	exit(1);
}

void init_server(int port)
{
	g_server.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (g_server.fd < 0)
		exit_fatal();

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	addr.sin_port = htons(port);

	if (bind(g_server.fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0)
		exit_fatal();
	if (listen(g_server.fd, CONNECTION_QUEUE_SIZE) != 0)
		exit_fatal();

	for (int i = 0; i < MAX_CLIENTS; ++i)
		g_server.poll_fds[i].fd = -1;

	g_server.largest_fd = g_server.fd;
	g_server.poll_fds[g_server.fd].fd = g_server.fd;
	g_server.poll_fds[g_server.fd].events = POLLIN;
}

void broadcast_msg(char *msg, int size, int sender_fd)
{
	t_client *client;

	for (int i = 0; i <= g_server.largest_fd; ++i)
	{
		client = &g_clients[i];

		if (i == sender_fd || !client->connected || !(g_server.poll_fds[i].revents & POLLOUT))
			continue ;

		send(i, msg, size, MSG_NOSIGNAL);
	}
}

void receive_client(void)
{
	if (!(g_server.poll_fds[g_server.fd].revents & POLLIN))
		return ;

	int new_fd = accept(g_server.fd, NULL, NULL);
	if (new_fd < 0)
		return ;
	if (new_fd >= MAX_CLIENTS)
	{
		close(new_fd);
		return ;
	}

	if (new_fd > g_server.largest_fd)
		g_server.largest_fd = new_fd;

	g_clients[new_fd].id = g_server.next_id++;
	g_clients[new_fd].connected = 1;
	g_clients[new_fd].buf_used = 0;

	g_server.poll_fds[new_fd].fd = new_fd;
	g_server.poll_fds[new_fd].events = POLLIN | POLLOUT;

	int msg_size = sprintf(g_server.buf, "server: client %d just arrived\n", g_clients[new_fd].id);
	broadcast_msg(g_server.buf, msg_size, new_fd);
}

void disconnect_client(int fd)
{
	t_client *client = &g_clients[fd];

	if (!client->connected)
		return ;

	client->connected = 0;
	client->buf_used = 0;

	g_server.poll_fds[fd].events = 0;
	g_server.poll_fds[fd].fd = -1;

	close(fd);

	if (fd == g_server.largest_fd)
	{
		int i = fd - 1;
		while (i > g_server.fd && !g_clients[i].connected)
			--i;
		g_server.largest_fd = i;
	}

	int msg_size = sprintf(g_server.buf, "server: client %d just left\n", client->id);
	broadcast_msg(g_server.buf, msg_size, fd);
}

void receive_msg(int fd)
{
	if (!(g_server.poll_fds[fd].revents & POLLIN))
		return ;

	t_client *client = &g_clients[fd];

	int buf_room = CLIENT_BUFFER_SIZE - client->buf_used - 1;
	if (buf_room <= 0)
	{
		disconnect_client(fd);
		return ;
	}

	int size = recv(fd, client->buf + client->buf_used, buf_room, 0);
	if (size <= 0)
	{
		disconnect_client(fd);
		return ;
	}

	client->buf_used += size;
	client->buf[client->buf_used] = '\0';
}

int extract_msg(char *str, int size)
{
	int pos = 0;
	while (pos < size && str[pos] != '\n')
		++pos;

	if (pos == size)
		return (0);

	str[pos] = '\0';
	return (pos + 1);
}

void shift_str(char *str, int size, int shift)
{
	for (int i = 0; i < size - shift; ++i)
		str[i] = str[i + shift];
	str[size - shift] = '\0';
}

void send_msgs(int fd)
{
	t_client *client = &g_clients[fd];
	if (!client->connected)
		return ;

	while (client->buf_used > 0)
	{
		int client_msg_size = extract_msg(client->buf, client->buf_used);
		if (client_msg_size <= 0)
			break ;

		int msg_size = sprintf(g_server.buf, "client %d: %s\n", client->id, client->buf);
		broadcast_msg(g_server.buf, msg_size, fd);

		shift_str(client->buf, client->buf_used, client_msg_size);
		client->buf_used -= client_msg_size;
	}
}

void route_msgs(void)
{
	for (int i = 0; i <= g_server.largest_fd; ++i)
	{
		if (!g_clients[i].connected)
			continue ;
		receive_msg(i);
		send_msgs(i);
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write (STDERR_FILENO, "Wrong number of arguments\n", 26);
		return (1);
	}

	init_server(atoi(argv[1]));

	while (1)
	{
		poll(g_server.poll_fds, g_server.largest_fd + 1, -1);
		receive_client();
		route_msgs();
	}

	if (g_server.fd >= 0)
		close(g_server.fd);
	return (0);
}
