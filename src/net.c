/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
/* src/net.c */
#include "../include/net.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Initialize networking subsystem */
int
netInit(void)
{
    return 0;
}

/* Create a new socket */
int
netCreateSocket(struct socket_info *sock, enum socket_type type)
{
    int fd;
    int flags;

    if (sock == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Create socket based on type */
    fd = socket(AF_INET,
               (type == SOCKET_TCP) ? SOCK_STREAM : SOCK_DGRAM,
               0);
    if (fd == -1) {
        return -1;
    }

    /* Set socket to non-blocking */
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        close(fd);
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        close(fd);
        return -1;
    }

    /* Initialize socket info structure */
    memset(sock, 0, sizeof(struct socket_info));
    sock->fd = fd;
    sock->type = type;
    sock->state = SOCKET_INIT;
    sock->backlog = NET_MAX_BACKLOG;

    return 0;
}

/* Bind socket to address and port */
int
netBind(struct socket_info *sock, const char *host, int port)
{
    int opt;
    uint16_t net_port;

    /* Input validation */
    if (sock == NULL || host == NULL || port <= 0 || port > 65535) {
        errno = EINVAL;
        return -1;
    }

    /* Validate port range and convert to network byte order */
    if (port > UINT16_MAX) {
        errno = ERANGE;
        return -1;
    }
    net_port = (uint16_t)port;

    /* Set socket options */
    opt = 1;
    if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        return -1;
    }

    /* Configure socket address */
    memset(&sock->addr, 0, sizeof(sock->addr));
    sock->addr.sin_family = AF_INET;
    sock->addr.sin_port = htons(net_port);

    /* Convert IP address */
    if (inet_pton(AF_INET, host, &sock->addr.sin_addr) != 1) {
        return -1;
    }

    /* Bind socket */
    if (bind(sock->fd, (struct sockaddr *)&sock->addr, sizeof(sock->addr)) == -1) {
        return -1;
    }

    sock->state = SOCKET_BOUND;
    return 0;
}

/* Listen for incoming connections */
int
netListen(struct socket_info *sock)
{
    if (sock == NULL || sock->state != SOCKET_BOUND) {
        return -1;
    }

    if (listen(sock->fd, sock->backlog) == -1) {
        errorLog(ERROR_CRITICAL, "Failed to listen on socket");
        return -1;
    }

    sock->state = SOCKET_LISTENING;
    return 0;
}

/* Accept incoming connection */
int
netAccept(struct socket_info *server, struct socket_info *client)
{
    socklen_t addr_len;
    int client_fd;

    if (server == NULL || client == NULL ||
        server->state != SOCKET_LISTENING) {
        return -1;
    }

    addr_len = sizeof(client->addr);
    client_fd = accept(server->fd,
                      (struct sockaddr *)&client->addr,
                      &addr_len);

    if (client_fd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            errorLog(ERROR_WARNING, "Failed to accept connection");
        }
        return -1;
    }

    client->fd = client_fd;
    client->state = SOCKET_CONNECTED;
    client->type = SOCKET_TCP;

    return 0;
}

/* Close socket */
void
netClose(struct socket_info *sock)
{
    if (sock == NULL) {
        return;
    }

    if (sock->fd != -1) {
        close(sock->fd);
        sock->fd = -1;
    }

    sock->state = SOCKET_CLOSED;
}

/* Clean up networking subsystem */
void
netCleanup(void)
{
    /* Add any necessary cleanup code */
}
