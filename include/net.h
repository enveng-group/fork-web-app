/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/net.h */
#ifndef NET_H
#define NET_H

#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>

/* Constants */
#define NET_MAX_BACKLOG 128
#define NET_DEFAULT_PORT 8080
#define NET_MAX_CONNECTIONS 1024

/* Socket states */
enum socket_state {
    SOCKET_INIT,
    SOCKET_BOUND,
    SOCKET_LISTENING,
    SOCKET_CONNECTED,
    SOCKET_ERROR,
    SOCKET_CLOSED
};

/* Socket types */
enum socket_type {
    SOCKET_TCP,
    SOCKET_UDP
};

/* Socket structure */
struct socket_info {
    int fd;                     /* Socket file descriptor */
    enum socket_state state;    /* Current socket state */
    enum socket_type type;      /* Socket type (TCP/UDP) */
    struct sockaddr_in addr;    /* Socket address */
    int backlog;               /* Listen backlog for TCP */
};

/* Function prototypes */
int netInit(void);
int netCreateSocket(struct socket_info *sock, enum socket_type type);
int netBind(struct socket_info *sock, const char *host, int port);
int netListen(struct socket_info *sock);
int netAccept(struct socket_info *server, struct socket_info *client);
void netClose(struct socket_info *sock);
void netCleanup(void);

#endif /* NET_H */
