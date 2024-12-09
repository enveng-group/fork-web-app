/**
 * \file server.c
 * \brief Implements server-specific functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "server.h"
#include "../include/logger.h"
#include "../include/http_response.h"
#include <unistd.h>

void *handleClient(void *arg)
{
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[RESPONSE_BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0)
    {
        /* Handle client request */
        logInfo("Received data from client: %s", buffer);
    }

    close(client_fd);
    return NULL;
}
