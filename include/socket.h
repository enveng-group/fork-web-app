/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/socket.h */
#ifndef SOCKET_H
#define SOCKET_H

#include "common.h"
#include "net.h"
#include "tcp.h"

/* Socket module types and constants */
#define SOCKET_MAX_BACKLOG 128
#define SOCKET_MAX_CONNECTIONS 1024
#define SOCKET_TIMEOUT_SEC 30
#define SOCKET_BUFFER_SIZE 4096

/* Socket module error codes */
enum socket_error {
    SOCKET_SUCCESS = 0,
    SOCKET_ERROR_INIT = -1,
    SOCKET_ERROR_BIND = -2,
    SOCKET_ERROR_LISTEN = -3,
    SOCKET_ERROR_ACCEPT = -4,
    SOCKET_ERROR_SEND = -5,
    SOCKET_ERROR_RECV = -6,
    SOCKET_ERROR_CLOSE = -7
};

/* Socket module initialization flags */
enum socket_init_flags {
    SOCKET_INIT_DEFAULT = 0,
    SOCKET_INIT_NONBLOCK = 1,
    SOCKET_INIT_REUSE_ADDR = 2
};

/* Function prototypes */
int socketInit(enum socket_init_flags flags);
void socketCleanup(void);
int socketGetLastError(void);
const char *socketGetErrorStr(int error);

#endif /* SOCKET_H */
