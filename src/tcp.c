/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/tcp.c */
#include "../include/tcp.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <arpa/inet.h>

/* Initialize TCP subsystem */
int
tcpInit(void)
{
    return netInit();
}

/* Connect to remote host */
int
tcpConnect(struct tcp_connection *conn, const char *host, int port)
{
    uint16_t net_port;

    if (conn == NULL || host == NULL || port <= 0 || port > UINT16_MAX) {
        errno = EINVAL;
        return -1;
    }

    /* Convert port to network byte order safely */
    net_port = (uint16_t)port;

    /* Create socket */
    if (netCreateSocket(&conn->sock, SOCKET_TCP) != 0) {
        return -1;
    }

    /* Set up address structure */
    conn->sock.addr.sin_family = AF_INET;
    conn->sock.addr.sin_port = htons(net_port);
    if (inet_pton(AF_INET, host, &conn->sock.addr.sin_addr) != 1) {
        netClose(&conn->sock);
        return -1;
    }

    /* Connect */
    if (connect(conn->sock.fd,
                (struct sockaddr *)&conn->sock.addr,
                sizeof(conn->sock.addr)) == -1) {
        if (errno != EINPROGRESS) {
            netClose(&conn->sock);
            return -1;
        }
    }

    /* Store connection info */
    strncpy(conn->remote_addr, host, INET_ADDRSTRLEN - 1);
    conn->remote_addr[INET_ADDRSTRLEN - 1] = '\0';
    conn->remote_port = port;
    conn->state = TCP_STATE_CONNECTED;
    time(&conn->last_activity);

    return 0;
}

/* Send data over TCP connection */
int
tcpSend(struct tcp_connection *conn, const void *data, size_t size)
{
    const unsigned char *ptr;
    size_t remaining;
    ssize_t sent;
    size_t total_sent;

    if (conn == NULL || data == NULL || size == 0) {
        errno = EINVAL;
        return -1;
    }

    ptr = (const unsigned char *)data;
    remaining = size;
    total_sent = 0;

    while (remaining > 0) {
        sent = send(conn->sock.fd, ptr, remaining, 0);
        if (sent == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            return -1;
        }

        if (sent > 0) {
            ptr += sent;
            if ((size_t)sent <= remaining) {
                remaining -= (size_t)sent;
                total_sent += (size_t)sent;
            }
        }

        time(&conn->last_activity);
    }

    if (total_sent > INT_MAX) {
        errno = EOVERFLOW;
        return -1;
    }

    return (int)total_sent;
}

/* Receive data from TCP connection */
int
tcpReceive(struct tcp_connection *conn, void *buffer, size_t size)
{
    ssize_t received;

    if (conn == NULL || buffer == NULL || size == 0) {
        return -1;
    }

    received = recv(conn->sock.fd, buffer, size, 0);
    if (received == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return -1;
    }

    if (received == 0) {
        conn->state = TCP_STATE_CLOSED;
        return 0;
    }

    time(&conn->last_activity);
    return (int)received;
}

/* Close TCP connection */
void
tcpClose(struct tcp_connection *conn)
{
    if (conn == NULL) {
        return;
    }

    netClose(&conn->sock);
    conn->state = TCP_STATE_CLOSED;
}

/* Clean up TCP subsystem */
void
tcpCleanup(void)
{
    netCleanup();
}
