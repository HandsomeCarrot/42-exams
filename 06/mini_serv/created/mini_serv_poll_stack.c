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
		close(g_server.fd);

	exit(1);
}

/**
 * \brief Creates, binds and listens on the server socket.
 *
 * Creates a TCP (AF_INET, SOCK_STREAM) socket bound to 127.0.0.1
 * on the given port and puts it into listening state with a backlog
 * of CONNECTION_QUEUE_SIZE. Initializes `g_server.poll_fds`
 * (all slots to fd -1, listening fd to POLLIN),
 * `g_server.largest_fd` and relies on static zero-initialization
 * for `g_server.next_id`.
 *
 * \param port Port number in host byte order (already converted
 * with atoi by the caller, converted to network order with htons).
 *
 * \note Calls exit_fatal() if socket(), bind() or listen() fails.
 */
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

/**
 * \brief Sends a message to every connected client except the sender.
 *
 * Iterates over `[0, g_server.largest_fd]` and calls send() with
 * MSG_NOSIGNAL on each connected client that is marked writable
 * (`POLLOUT` in `g_server.poll_fds[i].revents`), skipping `sender_fd`.
 *
 * \param msg Buffer holding the bytes to send.
 * \param size Number of bytes to send from `msg`.
 * \param sender_fd File descriptor to exclude (sender, or listening
 * socket fd when the message originates from the server itself).
 *
 * \note Send errors are ignored.
 */
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

/**
 * \brief Accepts a pending connection on the listening socket.
 *
 * Does nothing unless the listening socket `g_server.fd` is marked
 * readable (`POLLIN` in its `revents`). Otherwise calls accept();
 * on success initializes the `g_clients[new_fd]` slot
 * (`connected = 1`, `id = g_server.next_id++`, `buf_used = 0`),
 * registers `new_fd` in `g_server.poll_fds` with
 * `events = POLLIN | POLLOUT`, updates `g_server.largest_fd`
 * if needed, and broadcasts "server: client %d just arrived\n".
 *
 * Connections with `new_fd < 0` are ignored, and connections with
 * `new_fd >= MAX_CLIENTS` are immediately closed to stay within
 * the `g_clients` / `g_server.poll_fds` tables.
 *
 * \note Unlike the select implementation, registration is done
 * inline here instead of a separate register_client() helper.
 */
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

/**
 * \brief Disconnects a client and notifies the others.
 *
 * Marks `g_clients[fd]` as disconnected, clears its buffer state,
 * removes `fd` from `g_server.poll_fds` (`fd = -1`, `events = 0`),
 * closes it, and broadcasts "server: client %d just left\n".
 * If `fd` was `g_server.largest_fd`, scans downwards to recompute
 * the new `g_server.largest_fd`.
 *
 * \param fd File descriptor (index into `g_clients`) of the client
 * to disconnect.
 */
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

/**
 * \brief Reads available bytes from a client into its buffer.
 *
 * Does nothing unless `fd` is marked readable (`POLLIN` in
 * `g_server.poll_fds[fd].revents`). Otherwise appends up to the
 * remaining free space (`CLIENT_BUFFER_SIZE - buf_used - 1`,
 * reserving one byte for the terminating '\0') via recv() and
 * NUL-terminates the buffer.
 *
 * \param fd File descriptor (index into `g_clients`) to read from.
 *
 * \note Disconnects the client via disconnect_client() if the buffer
 * is full or if recv() returns <= 0 (error or orderly shutdown).
 */
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
 * newline included but replaced by '\0'). Returns `0` if no newline
 * character '\n' was found within the first `size` bytes.
 */
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

/**
 * \brief Shifts a string left by `shift` characters.
 *
 * Copies the substring starting at `str[shift]` to the beginning of
 * `str` and NUL-terminates the result. The first `shift` characters
 * are effectively discarded.
 *
 * \param str The string to shift in place.
 * \param size The size of the buffered data, used as an upper bound.
 * \param shift Number of characters to remove from the front.
 */
void shift_str(char *str, int size, int shift)
{
	for (int i = 0; i < size - shift; ++i)
		str[i] = str[i + shift];
	str[size - shift] = '\0';
}

/**
 * \brief Flushes complete lines from a client buffer to all others.
 *
 * Repeatedly extracts newline-terminated messages with
 * extract_msg(); each complete message is formatted as
 * "client %d: %s\n" into `g_server.buf` and relayed with
 * broadcast_msg(). Consumed bytes are removed with shift_str()
 * and `buf_used` is adjusted. Stops when the buffer is empty or
 * no complete line remains.
 *
 * \param fd File descriptor (index into `g_clients`) whose buffer
 * is flushed.
 *
 * \note Does nothing if the client is not connected.
 */
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

/**
 * \brief Pumps I/O for every connected client.
 *
 * Iterates over `[0, g_server.largest_fd]`; for each connected client
 * calls receive_msg() followed by send_msgs(), so newly arrived
 * bytes are read and any complete lines are broadcast.
 */
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

/**
 * \brief Program entry point: runs the chat server event loop.
 *
 * Expects exactly one argument (the port). Initializes the server
 * with init_server(), then loops forever: poll() on
 * `[0, g_server.largest_fd]`, receive_client(), route_msgs().
 *
 * The poll() call blocks indefinitely (timeout -1) over
 * `g_server.largest_fd + 1` entries; its return value is
 * intentionally ignored.
 *
 * \param argc Argument count; must be 2.
 * \param argv Argument vector; argv[1] is the port string.
 *
 * \return 0 on normal termination (unreachable in practice: the
 * event loop never exits; misuse prints "Wrong number of
 * arguments\n" and returns 1).
 */
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
