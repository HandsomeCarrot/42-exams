#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

enum constants
{
	MAX_CLIENTS = FD_SETSIZE,
	CLIENT_BUFFER_SIZE = 1024, // could be better if set to a higher value (e.g. 2^16 = 65536)
	SERVER_MSGS_MAX_SIZE = 40,
	SERVER_BUFFER_SIZE = (CLIENT_BUFFER_SIZE + SERVER_MSGS_MAX_SIZE),
	CONNECTION_QUEUE_SIZE = 10
}	;

typedef struct client
{
	int		id;
	int		connected;
	int		buffer_used;
	char	buffer[CLIENT_BUFFER_SIZE];
}			t_client;

typedef struct server
{
	int		fd;
	int		max_fd;
	int		next_id;
	fd_set	all_fds;
	fd_set	r_fds;
	fd_set	w_fds;
	char	buffer[SERVER_BUFFER_SIZE];
}			t_server;

t_server	g_server;
t_client	g_clients[MAX_CLIENTS];

/**
 * \brief Terminates the program with a fatal error.
 *
 * Writes "Fatal error\n" to STDERR, closes the server socket (if valid)
 * and exits with status 1.
 * Called whenever an unrecoverable error occurs (e.g. socket,
 * bind or listen failure).
 */
void exit_fatal(void)
{
	write(STDERR_FILENO, "Fatal error\n", 12);

	if (g_server.fd >= 0)
		close (g_server.fd);

	exit(1);
}

/**
 * \brief Creates, binds and listens on the server socket.
 *
 * Creates a TCP (AF_INET, SOCK_STREAM) socket bound to 127.0.0.1
 * on the given port and puts it into listening state with a backlog
 * of CONNECTION_QUEUE_SIZE. Initializes `g_server.max_fd`,
 * `g_server.next_id` and `g_server.all_fds`.
 *
 * \param port Null-terminated string holding the port number
 * (converted with atoi and htons).
 *
 * \note Calls exit_fatal() if socket(), bind() or listen() fails.
 */
void init_server(char *port)
{
	g_server.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (g_server.fd < 0)
		exit_fatal();

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	addr.sin_port = htons(atoi(port));

	if (bind(g_server.fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0)
		exit_fatal();

	if (listen(g_server.fd, CONNECTION_QUEUE_SIZE) == -1)
		exit_fatal();

	g_server.max_fd = g_server.fd;
	g_server.next_id = 0;
	FD_ZERO(&g_server.all_fds);
	FD_SET(g_server.fd, &g_server.all_fds);
}

/**
 * \brief Polls all tracked file descriptors for I/O readiness.
 *
 * Copies `g_server.all_fds` into `g_server.r_fds` and
 * `g_server.w_fds`, then blocks in select() on
 * `[0, g_server.max_fd]` until at least one descriptor is
 * readable and/or writable. The resulting sets drive
 * receive_clients(), receive_bytes() and broadcast_msg().
 *
 * \note Blocks indefinitely (no timeout); return value of select()
 * is intentionally ignored.
 */
void scan_events(void)
{
	g_server.r_fds = g_server.all_fds;
	g_server.w_fds = g_server.all_fds;

	select(g_server.max_fd + 1, &g_server.r_fds, &g_server.w_fds, NULL, NULL);
}

/**
 * \brief Sends a message to every connected client except the sender.
 *
 * Iterates over `[0, g_server.max_fd]` and calls send() with
 * MSG_NOSIGNAL on each connected client that is marked writable
 * in `g_server.w_fds`, skipping `sender_fd`.
 *
 * \param msg Buffer holding the bytes to send.
 * \param size Number of bytes to send from `msg`.
 * \param sender_fd File descriptor to exclude (sender, or listening
 * socket fd when the message originates from the server itself).
 *
 * \note Does nothing if `msg` is NULL or `size < 1`. Send errors
 * are ignored.
 */
void broadcast_msg(const char *msg, int size, int sender_fd)
{
	if (!msg || size < 1)
		return ;

	t_client *client;
	for (int i = 0; i <= g_server.max_fd; ++i)
	{
		client = &g_clients[i];

		if (!client->connected || i == sender_fd || !FD_ISSET(i, &g_server.w_fds))
			continue ;

		send(i, msg, size, MSG_NOSIGNAL);
	}
}

/**
 * \brief Disconnects a client and notifies the others.
 *
 * Marks `g_clients[fd]` as disconnected, removes `fd` from
 * `g_server.all_fds`, closes it, and broadcasts
 * "server: client %d just left\n". If `fd` was `g_server.max_fd`,
 * scans downwards to recompute the new `g_server.max_fd`.
 *
 * \param fd File descriptor (index into `g_clients`) of the client
 * to disconnect.
 */
void disconnect_client(int fd)
{
	g_clients[fd].connected = 0;
	FD_CLR(fd, &g_server.all_fds);
	close(fd);

	int msg_size = sprintf(g_server.buffer, "server: client %d just left\n", g_clients[fd].id);
	broadcast_msg(g_server.buffer, msg_size, fd);

	if (fd != g_server.max_fd)
		return ;

	g_server.max_fd = g_server.fd;

	for (int i = (fd - 1); i > g_server.fd; --i)
	{
		if (g_clients[i].connected)
		{
			g_server.max_fd = i;
			break ;
		}
	}
}

/**
 * \brief Registers a newly accepted client.
 *
 * Initializes the `g_clients[fd]` slot: sets `connected = 1`,
 * assigns `id = g_server.next_id++`, clears `buffer_used`, adds
 * `fd` to `g_server.all_fds`, updates `g_server.max_fd` if needed,
 * and broadcasts "server: client %d just arrived\n".
 *
 * \param fd File descriptor (index into `g_clients`) of the new
 * client, as returned by accept().
 */
void register_client(int fd)
{
	t_client *client = &g_clients[fd];

	client->connected = 1;
	client->id = g_server.next_id++;
	client->buffer_used = 0;
	FD_SET(fd, &g_server.all_fds);

	if (fd > g_server.max_fd)
		g_server.max_fd = fd;

	int size = sprintf(g_server.buffer, "server: client %d just arrived\n", client->id);
	broadcast_msg(g_server.buffer, size, fd);
}

/**
 * \brief Accepts pending connections on the listening socket.
 *
 * Does nothing unless the listening socket `g_server.fd` is marked
 * readable in `g_server.r_fds`. Otherwise calls accept(); on success
 * registers the client via register_client(). Connections with
 * `client_fd < 0` are ignored, and connections with
 * `client_fd >= MAX_CLIENTS` are immediately closed to stay within
 * the `g_clients` table.
 */
void receive_clients(void)
{
	if (!FD_ISSET(g_server.fd, &g_server.r_fds))
		return ;

	int client_fd = accept(g_server.fd, NULL, NULL);

	if (client_fd < 0)
		return ;
	if (client_fd >= MAX_CLIENTS)
	{
		close(client_fd);
		return ;
	}

	register_client(client_fd);
}

/**
 * \brief Reads available bytes from a client into its buffer.
 *
 * Does nothing unless `client_fd` is marked readable in
 * `g_server.r_fds`. Otherwise appends up to the remaining free
 * space (`CLIENT_BUFFER_SIZE - buffer_used - 1`, reserving one byte
 * for the terminating '\0') via recv() and NUL-terminates the buffer.
 *
 * \param client_fd File descriptor (index into `g_clients`) to read
 * from.
 *
 * \note Disconnects the client via disconnect_client() if the buffer
 * is full or if recv() returns <= 0 (error or orderly shutdown).
 */
void receive_bytes(int client_fd)
{
	if (!FD_ISSET(client_fd, &g_server.r_fds))
		return ;

	t_client *client = &g_clients[client_fd];

	int buffer_room = CLIENT_BUFFER_SIZE - client->buffer_used - 1;

	if (buffer_room <= 0)
	{
		disconnect_client(client_fd);
		return ;
	}

	ssize_t received = recv(client_fd, (client->buffer + client->buffer_used), buffer_room, 0);

	if (received <= 0)
	{
		disconnect_client(client_fd);
		return ;
	}

	client->buffer_used += received;
	client->buffer[client->buffer_used] = '\0';
}

/**
 * \brief 'Extracts' a message from the string.
 *
 * Scans `str` for the first newline character '\n' within the first
 * `size` bytes and replaces it with a null byte '\0', effectively
 * terminating the message in place at that point.
 *
 * \param str - the string from which the message will be extracted.
 * \param size - the size of the given string (upper scan bound).
 *
 * \return The number of bytes consumed by the extracted message: the
 * offset of the '\n' plus one (i.e. the length of the message,
 * newline included but replaced by '\0'). Returns `0` if:
 * - str == NULL.
 * - no newline character '\n' was found within the first `size` bytes.
 */
int extract_message(char *str, int size)
{
	if (!str)
		return (0);

	int	newline_pos = 0;

	while (newline_pos < size && str[newline_pos] != '\n')
		++newline_pos;

	if (newline_pos == size)
		return (0); // nothing found

	str[newline_pos] = '\0';
	return (newline_pos + 1);
}

/**
 * \brief Shifts a string left by `shift` characters.
 *
 * Copies the substring starting at `str[shift]` to the beginning of
 * `str`. The first `shift` characters are effectively discarded.
 *
 * \note Does nothing if `str` is NULL or if `shift >= size`.
 *
 * \param str The string to shift in place.
 * \param size The size of the buffer, used as an upper bound.
 * \param shift Number of characters to remove from the front.
 */
void shift_string(char *str, size_t size, size_t shift)
{
	if (!str || shift >= size)
		return ;

	for (int i = 0; (i + shift) < size; ++i)
		str[i] = str[i + shift];
}

/**
 * \brief Flushes complete lines from a client buffer to all others.
 *
 * Repeatedly extracts newline-terminated messages with
 * extract_message(); each complete message is formatted as
 * "client %d: %s\n" into `g_server.buffer` and relayed with
 * broadcast_msg(). Consumed bytes are removed with shift_string()
 * and `buffer_used` is adjusted. Stops when the buffer is empty or
 * no complete line remains.
 *
 * \param client_fd File descriptor (index into `g_clients`) whose
 * buffer is flushed.
 *
 * \note Does nothing if the client is not connected.
 */
void send_bytes(int client_fd)
{
	t_client *client = &g_clients[client_fd];
	int str_size;

	if (!client->connected)
		return ;

	while (client->buffer_used > 0)
	{
		str_size = extract_message(client->buffer, client->buffer_used);

		if (str_size <= 0)
			return ;

		int msg_size = sprintf(g_server.buffer, "client %d: %s\n", client->id, client->buffer);
		broadcast_msg(g_server.buffer, msg_size, client_fd);

		shift_string(client->buffer, client->buffer_used, str_size);
		client->buffer_used -= str_size;
		client->buffer[client->buffer_used] = '\0';
	}
}

/**
 * \brief Pumps I/O for every connected client.
 *
 * Iterates over `[0, g_server.max_fd]`; for each connected client
 * calls receive_bytes() followed by send_bytes(), so newly arrived
 * bytes are read and any complete lines are broadcast.
 */
void route_msgs(void)
{
	for (int i = 0; i <= g_server.max_fd; ++i)
	{
		if (!g_clients[i].connected)
			continue ;

		receive_bytes(i);
		send_bytes(i);
	}
}

/**
 * \brief Program entry point: runs the chat server event loop.
 *
 * Expects exactly one argument (the port). Initializes the server
 * with init_server(), then loops forever: scan_events(),
 * receive_clients(), route_msgs().
 *
 * \param argc Argument count; must be 2.
 * \param argv Argument vector; argv[1] is the port string.
 *
 * \return 0 on normal termination (unreachable in practice: the
 * event loop never exits; misuse prints "Wrong number of
 * arguments\n" and exits with status 1).
 */
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(STDERR_FILENO, "Wrong number of arguments\n", 26);
		exit(1);
	}

	init_server(argv[1]);

	while (1)
	{
		scan_events();
		receive_clients();
		route_msgs();
	}

	exit(0);
}
