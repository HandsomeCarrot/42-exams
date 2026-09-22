# mini_serv

## status

failed (2026-09-22)
> there can be small differences in the code and subject that I did not notice

> tries: 2

## toc

- [subject](#subject)
- [file structure](#file-structure)
- [usage](#usage)
- [personal notes](#personal-notes)

## subject

Assignment name  : mini_serv

Expected files   : mini_serv.c

Allowed functions: write, close, select, poll, socket, accept, listen, send, recv, bind, strstr, malloc, realloc, free, calloc, bzero, atoi, sprintf, strlen, exit, strcpy, strcat, memset, htons, htonl

Write a program that will listen for client to connect on a certain port on 127.0.0.1 and will let clients to speak with each other.

This program will take as first argument the port to bind to.
If no argument is given, it should write in stderr "Wrong number of arguments" followed by a \n and exit with status 1.
If a System Call returns an error before the program starts accepting connections, it should write in stderr "Fatal error" followed by a \n and exit with status 1.
If you cant allocate memory it should write in stderr "Fatal error" followed by a \n and exit with status 1.

Your program must be non-blocking but client can be lazy and if they don't read your message you must NOT disconnect them.

Your program must not contain #define preproc.
Your program must only listen to 127.0.0.1.
The fd that you will receive will already be set to make 'recv' or 'send' to block if select hasn't be called before calling them, but will not block otherwise.

When a client connect to the server:
- the client will be given an id. the first client will receive the id 0 and each new client will received the last client id + 1
- %d will be replace by this number
- a message is sent to all the client that was connected to the server: "server: client %d just arrived\n"

clients must be able to send messages to your program.
- message will only be printable characters, no need to check
- a single message can contain multiple `\n` characters
- when the server receives a message, it must resend it to all the other clients with "client %d: %s" before every line!

When a client disconnect from the server:
- a message is sent to all the client that was connected to the server: "server: client %d just left\n"

Memory or fd leaks are forbidden.

To help you, you will find the file main.c with the beginning of a server and maybe some useful functions. (Beware this file use forbidden functions or write things that must not be there in your final program)

Warning our tester is expecting that you send the messages as fast as you can. Don't do un-necessary buffer.

Evaluation can be a bit longer than usual.

Hint: you can use nc to test your program.
Hint: you should use nc to test your program.
Hint: To test you can use fcntl(fd, F_SETFL, O_NONBLOCK) but use select and NEVER check EAGAIN (man 2 send).

## file structure

The subject is in this readme and not in a separate file. All other files are organized as follows:

### created

Files created and implemented for this exercise:

- `mini_serv_select.c` — implementation using `select`
- `mini_serv_poll.c` — implementation using `poll`

Both implementations are functionally identical. The only difference is the multiplexing mechanism used by the main loop (see [select vs poll](#select-vs-poll)). Both files are documented with Doxygen comments in the same style.

### given

Files you will be given for this exercise, but should not be pushed:

- `main.c` — starter code with a socket/bind/accept skeleton plus `extract_message` / `str_join` helpers (uses forbidden functions, do not copy verbatim into the final program)

### root

All files in here are extra files, that you do not need at all for this exercise:

- `Makefile` — builds `serv_select` (default `make` / `make serv_select`) and `serv_poll` (`make serv_poll`), plus `clean` / `fclean` / `re`

## usage

Compile the `select` implementation:

```bash
make
# or
make serv_select
./serv_select 8081
```

Compile the `poll` implementation:

```bash
make serv_poll
./serv_poll 8081
```

Connect with (one `nc` per client, from another terminal):

```bash
nc 127.0.0.1 8081
```

Then type lines + Enter. Each line is relayed to all other clients as `client <id>: <line>`. Connects/disconnects are announced as `server: client <id> just arrived` / `server: client <id> just left`.

## personal notes

- Two implementations are available: one uses `select`, the other uses `poll`.
- Apart from the multiplexing function, both implementations are almost identical.
- One listening socket is bound to `127.0.0.1`.
- Client IDs use the arrival order, starting at `0`.
- Client messages are buffered per client and broadcast line by line.
- Connect and disconnect notices are sent to the other clients.
- Everything is stored on the stack / in static storage, so there are no memory leaks.

### logic flow

- parse the port argument, create a TCP socket, bind it to `127.0.0.1`, listen with a small backlog
- loop forever:
  1. wait for readiness (`select` over `[0, max_fd]` in `scan_events`, or `poll` over `[0, largest_fd]` directly in `main`)
  2. accept pending clients (`receive_clients` / `receive_client`): assign `id = next_id++`, init per-client buffer, announce `server: client %d just arrived`
  3. pump each connected client (`route_msgs`): `recv` available bytes into its buffer (`receive_bytes` / `receive_msg`), then extract every complete `\n`-terminated line and broadcast it as `client %d: %s` (`send_bytes` / `send_msgs`)
  4. on `recv <= 0` or a full buffer, disconnect the client: close the fd, shrink `max_fd` / `largest_fd`, announce `server: client %d just left`
- partial lines (no `\n` yet) stay in the per-client buffer; a single `recv` containing multiple `\n` is split and broadcast line by line

### design decisions

- fd-indexed tables: `g_clients[fd]` and (`all_fds` / `poll_fds[fd]`) so lookup is O(1) and no malloc is needed
- per-client buffer of `CLIENT_BUFFER_SIZE` (1024), reserving one byte for `\0`; one shared server buffer of `CLIENT_BUFFER_SIZE + 40` for the `client %d: ` prefix
- broadcast only to clients flagged writable (`FD_ISSET(w_fds)` / `POLLOUT` in `revents`), send with `MSG_NOSIGNAL`, ignore send errors — this keeps the server non-blocking without disconnecting lazy clients
- no `#define`, only an `enum` for constants, to comply with the subject
- `sprintf` for message formatting; `bzero` / `htonl(2130706433)` for `127.0.0.1`, as in the given `main.c`
- broadcast directly out of the event loop into a single server buffer, no extra queueing, to satisfy the "send as fast as you can" tester note
