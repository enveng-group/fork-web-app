/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/socket.c */
#include "../include/socket.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

/* Static variables */
static int socket_initialized = 0;
static int last_error = SOCKET_SUCCESS;

/* Error messages corresponding to socket_error codes */
static const char *error_messages[] = {
    "Success",
    "Socket initialization failed",
    "Socket bind failed",
    "Socket listen failed",
    "Socket accept failed",
    "Socket send failed",
    "Socket receive failed",
    "Socket close failed"
};

int
socketInit(enum socket_init_flags flags)
{
    int status;

    /* Already initialized check */
    if (socket_initialized) {
        last_error = SOCKET_SUCCESS;
        return 0;
    }

    /* Initialize networking subsystem */
    status = netInit();
    if (status != 0) {
        last_error = SOCKET_ERROR_INIT;
        return -1;
    }

    /* Handle initialization flags */
    if (flags & SOCKET_INIT_NONBLOCK) {
        /* Non-blocking setup would go here */
    }

    if (flags & SOCKET_INIT_REUSE_ADDR) {
        /* Address reuse setup would go here */
    }

    socket_initialized = 1;
    last_error = SOCKET_SUCCESS;
    return 0;
}

void
socketCleanup(void)
{
    if (!socket_initialized) {
        return;
    }

    netCleanup();
    socket_initialized = 0;
    last_error = SOCKET_SUCCESS;
}

int
socketGetLastError(void)
{
    return last_error;
}

const char *
socketGetErrorStr(int error)
{
    size_t array_size;

    /* Handle success case specially since error code is 0 */
    if (error == SOCKET_SUCCESS) {
        return error_messages[0];
    }

    array_size = sizeof(error_messages) / sizeof(error_messages[0]);

    /* Check if error code is within valid range */
    if (error >= 0 || -error >= (int)array_size) {
        return "Unknown error";
    }

    return error_messages[-error];
}
