/**
 * \file server.h
 * \brief Header file for server-specific functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 100

typedef struct {
    int server_fd;
    struct sockaddr_in address;
    int addrlen;
} server_config_t;

void *handle_client(void *arg);

#endif /* SERVER_H */
