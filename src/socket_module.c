/**
 * \file socket_module.c
 * \brief Implements socket creation using POSIX socket API.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include "../include/constants.h"
#include "../include/logger.h"
#include "../include/socket_module.h"

/**
 * \brief Creates and binds a UDP socket.
 *
 * \return Created socket on success, -1 on failure.
 */
int createSocket(void)
{
    int                 sockfd;
    struct sockaddr_in  server_addr;
    int                 opt = 1;
    char                ip_str[INET_ADDRSTRLEN];

    /* Create UDP socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        logError("Failed to create socket: %s", strerror(errno));
        return -1;
    }

    /* Set socket options to allow reusing the address */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        logError("Failed to set socket options: %s", strerror(errno));
        close(sockfd);
        return -1;
    }

    /* Set up server address structure */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port        = htons(SERVER_PORT);

    /* Bind the socket to the server IP and port */
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        logError("Failed to bind socket: %s", strerror(errno));
        close(sockfd);
        return -1;
    }

    /* Convert IP address to string */
    inet_ntop(AF_INET, &server_addr.sin_addr, ip_str, sizeof(ip_str));
    logInfo("Socket created and bound to %s:%d", ip_str, ntohs(server_addr.sin_port));

    return sockfd;
}
