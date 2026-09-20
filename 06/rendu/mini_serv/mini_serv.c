#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct client
{
	int	    id;
	int		fd;
	char	*in_buf;
	int		in_buf_size;
	int		in_buf_used;
	int		disconnect;
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
	int					clients_to_disconnect;
	int					largest_fd;
	int					next_id;
}						t_server;

typedef enum msg_type
{
    JOIN_MSG,
    LEAVE_MSG,
    CLIENT_MSG
}   t_msg_type;

void cleanup(t_server *serv)
{
	// TODO
	(void)serv;
}

void exit_error(const char *msg)
{
	write(STDERR_FILENO, msg, strlen(msg));
	exit(1);
}

int extract_message(char **buf, char **msg, t_client *client)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, client->in_buf_size);
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			client->in_buf_used -= i;
			return (1);
		}
		i++;
	}
	return (0);
}

char *create_msg(int sender_id, t_msg_type type, const char *msg)
{
	char *complete_msg = NULL;
	int complete_msg_len = 10;
	int ret = 0;

	switch (type)
	{
		case JOIN_MSG:
			complete_msg_len += 29;
			break ;
		case LEAVE_MSG:
			complete_msg_len += 26;
			break ;
		case CLIENT_MSG:
			complete_msg_len += strlen(msg) + 9;
			break ;
	}


	complete_msg = calloc((complete_msg_len + 1), sizeof(char));
	if (!complete_msg)
		return (NULL);

	switch (type)
	{
		case JOIN_MSG:
			ret = sprintf(complete_msg, "server: client %d just arrived\n", sender_id);
			break ;
		case LEAVE_MSG:
			ret = sprintf(complete_msg, "server: client %d just left\n", sender_id);
			break ;
		case CLIENT_MSG:
			ret = sprintf(complete_msg, "client %d: %s", sender_id, msg);
			break ;
	}

	if (ret < 0)
		return (NULL);

	return (complete_msg);
}

void broadcast_msg(const char *msg, int sender_id, t_server *serv)
{
	size_t msg_len = strlen(msg);

	for (int i = 0; i < serv->clients_connected; ++i)
	{
	    if (serv->clients[i].id == sender_id)
			continue ;

		if ((send(serv->clients[i].fd, msg, msg_len, 0)) == -1)
			serv->clients[i].disconnect = 1;
	}
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

int register_client(t_server *serv, int new_fd)
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

	// debug
	printf("new client %d\n", new_client->id);

	return (new_client->id);
}

void handle_new_client(t_server *serv)
{
	int new_fd = accept(serv->fd, NULL, NULL);
	if (new_fd < 0)
		return;
	reserve_clients(serv);
	int new_id = register_client(serv, new_fd);

	char *msg = create_msg(new_id, JOIN_MSG	, NULL);
	broadcast_msg(msg,  new_id, serv);
	free(msg);
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
	if ((select(serv->largest_fd + 1, &serv->rfds, &serv->wfds, NULL, &wait_timer)) == -1)
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

			ssize_t rsize = recv(client->fd, (client->in_buf + client->in_buf_used), (client->in_buf_size - client->in_buf_used), 0);
			if (rsize == 0)
			{
				client->disconnect = 1;
				++serv.clients_to_disconnect;
			}
			else
				client->in_buf_used += rsize;
		}

		// remove disconnected clients
		for (int i = 0; (serv.clients_to_disconnect > 0 && i < serv.clients_connected); ++i)
		{
			client = &serv.clients[i];

			if (!client->disconnect)
				continue ;

			close(client->fd);
			free(client->in_buf);

			// debug
			printf("client left %d\n", client->id);

			char *msg = create_msg(client->id, LEAVE_MSG, NULL);
			broadcast_msg(msg,  client->id, &serv);
			free(msg);

			if (client->fd == serv.largest_fd)
			{
				serv.largest_fd = serv.fd;

				for (int j = 0; j < serv.clients_connected; ++j)
				{
					if (serv.clients[j].disconnect)
						continue ;
					if (serv.clients[j].fd > serv.largest_fd)
						serv.largest_fd = serv.clients[j].fd;
				}
			}

			for (int j = i; j + 1 < serv.clients_connected; ++j)
			{
				serv.clients[j].id = serv.clients[j+1].id;
				serv.clients[j].fd = serv.clients[j+1].fd;
				serv.clients[j].disconnect = serv.clients[j+1].disconnect;
				serv.clients[j].in_buf_size = serv.clients[j+1].in_buf_size;
				serv.clients[j].in_buf_used = serv.clients[j+1].in_buf_used;
				serv.clients[j].in_buf = serv.clients[j+1].in_buf;
			}

			bzero(&serv.clients[serv.clients_connected], sizeof(t_client));
			--serv.clients_to_disconnect;
			--serv.clients_connected;
		}

		// send received messages
		for (int i = 0; i < serv.clients_connected; ++i)
		{
			client = &serv.clients[i];

			if (client->in_buf_used <= 0)
				continue ;

			char *client_msg, *msg;
			int ret = extract_message(&client->in_buf, &client_msg, client);
			if (ret == -1)
				exit_error("Error: alloacation failed\n");
			else if (ret == 0)
				continue ;

			msg = create_msg(client->id, CLIENT_MSG, client_msg);
			free(client_msg);
			broadcast_msg(msg, client->id, &serv);
			free(msg);
		}
	}

	exit(0);
}
