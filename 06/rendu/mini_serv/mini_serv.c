#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct client
{
	int		id;
	int		fd;
	char	*in_buf;
	int		in_buf_size;
	int		in_buf_used;
	int		disconnected;
}			t_client;

typedef struct server
{
	int					fd;
	struct sockaddr_in	addr;
	fd_set				rfds;
	fd_set				wfds;
	t_client			*clients;
	int					clients_connected;
	int					clients_reserved;
	int					clients_disconnected;
	int					largest_fd;
	int					next_id;
}						t_server;

void exit_error(const char *msg)
{
	write(2, msg, strlen(msg));
	exit(1);
}

void cleanup(t_server *serv)
{
	// TODO
}

void broadcast_msg(const char *msg, int sender_id, t_server *serv)
{
	// TODO
	// NOTE: have to pre-allocate enough space for the string sprintf produces
}

void reserve_clients(t_server *serv)
{
	if (serv->clients_reserved > serv->clients_connected)
		return ;

	serv->clients_reserved = serv->clients_reserved == 0 ? 8 : (serv->clients_reserved * 2);
	t_client *tmp_clients = realloc(serv->clients, serv->clients_reserved * sizeof(t_client));
	if (!tmp_clients)
	{
		cleanup(serv);
		exit_error("Error: memory allocation failure\n");
	}
	serv->clients = tmp_clients;
}

void register_client(t_server *serv, int new_fd)
{
	int buf_max_chars = 250;
	char *new_buf = calloc(buf_max_chars, sizeof(char));
	if (!new_buf)
	{
		cleanup(serv);
		exit_error("Error: memory allocation failed\n");
	}

	t_client *new_client = &serv->clients[serv->clients_connected];
	bzero(new_client, sizeof(t_client));
	new_client->fd = new_fd;
	new_client->id = serv->next_id;
	new_client->in_buf = new_buf;
	new_client->in_buf_size = buf_max_chars * sizeof(char);

	++serv->clients_connected;
	++serv->next_id;
	if (new_client->fd > serv->largest_fd)
		serv->largest_fd = new_client->fd;
}

void handle_new_client(t_server *serv)
{
	int new_fd = accept(serv->fd, NULL, NULL);
	if (new_fd < 0)
		return;
	reserve_clients(serv);
	register_client(serv, new_fd);
	// TODO: broadcast join message
}

void init_server(t_server *serv, char *port)
{
	bzero(serv, sizeof(t_server));

	serv->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (serv->fd == -1)
		exit_error("Error: socket failed\n");

	serv->addr.sin_family = AF_INET;
	serv->addr.sin_addr.s_addr = htonl(2130706433);
	serv->addr.sin_port = htons(atoi(port));

	if (bind(serv->fd, (const struct sockaddr *) &serv->addr, sizeof(serv->addr)) == -1)
		exit_error("Error: bind failed\n");

	if (listen(serv->fd, 10) == -1)
		exit_error("Error: listen failed\n");

	serv->largest_fd = serv->fd;
	serv->next_id = 0;
}

void wait_for_events(t_server *serv)
{
	FD_ZERO(&serv->rfds);
	FD_ZERO(&serv->wfds);
	FD_SET(serv->fd, &serv->rfds);
	for (int i = 0; i < serv->clients_connected; ++i)
	{
		FD_SET(serv->clients[i].fd, &serv->rfds);
		FD_SET(serv->clients[i].fd, &serv->wfds);
	}

	struct timeval wait_timer = {0,0};
	if ((select(serv->largest_fd, &serv->rfds, &serv->wfds, NULL, &wait_timer)) == -1)
	{
		cleanup(serv);
		exit_error("Error: select failed\n");
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
		exit_error("Wrong number of arguments\n");

	t_server serv;

	init_server(&serv, argv[1]);

	while (1)
	{
		wait_for_events(&serv);

		if (FD_ISSET(serv.fd, &serv.rfds))
			handle_new_client(&serv);

		// receive messages from clients
		t_client *client;
		for (int i = 0; i < serv.clients_connected; ++i)
		{
			client = &serv.clients[i];

			if (!FD_ISSET(client->fd, &serv.rfds))
				continue ;

			ssize_t rsize = recv(client->fd, client->in_buf, (client->in_buf_size - client->in_buf_used), 0);
			if (rsize == 0)
			{
				client->disconnected = 1;
				++serv.clients_disconnected;
			}
			else
				client->in_buf_used += rsize;
		}

		// remove disconnected clients
		for (int i = 0; (serv.clients_disconnected > 0 && i < serv.clients_connected); ++i)
		{
			client = &serv.clients[i];

			if (!client->disconnected)
				continue ;

			close(client->fd);
			free(client->in_buf);
			// TODO: broadcast leave message
		}

		// send received messages
		for (int i = 0; i < serv.clients_connected; ++i)
		{
			client = &serv.clients[i];

			if (client->in_buf_used <= 0)
				continue ;

			// get message
			// broadcast message
		}
	}

	exit(0);
}