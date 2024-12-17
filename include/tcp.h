/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/tcp.h */
#ifndef TCP_H
#define TCP_H

#include "net.h"

/* Constants */
#define TCP_BUFFER_SIZE 4096
#define TCP_TIMEOUT_SEC 30

/* TCP connection states */
enum tcp_state {
    TCP_STATE_INIT,
    TCP_STATE_CONNECTED,
    TCP_STATE_ERROR,
    TCP_STATE_CLOSED
};

/* TCP connection info */
struct tcp_connection {
    struct socket_info sock;    /* Underlying socket */
    enum tcp_state state;       /* Connection state */
    char remote_addr[INET_ADDRSTRLEN]; /* Remote IP address */
    int remote_port;            /* Remote port */
    time_t last_activity;       /* Last activity timestamp */
};

/* Function prototypes */
int tcpInit(void);
int tcpConnect(struct tcp_connection *conn, const char *host, int port);
int tcpSend(struct tcp_connection *conn, const void *data, size_t size);
int tcpReceive(struct tcp_connection *conn, void *buffer, size_t size);
void tcpClose(struct tcp_connection *conn);
void tcpCleanup(void);

#endif /* TCP_H */
