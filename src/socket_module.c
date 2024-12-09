/**
 * \file socket_module.c
 * \brief Implements socket-related functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/socket_module.h"
#include "../include/logger.h"
#include "../include/constants.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * \brief Gets the server IP address.
 *
 * \return The server IP address as a string.
 */
const char *getServerIp(void)
{
    return "127.0.0.1"; /* Replace with actual logic to get the server IP */
}

/**
 * \brief Gets the server port.
 *
 * \return The server port as an integer.
 */
int getServerPort(void)
{
    return 8080; /* Replace with actual logic to get the server port */
}

/**
 * \brief Creates and binds a socket to the specified IP and port.
 *
 * \return The socket file descriptor on success, -1 on failure.
 */
int createAndBindSocket(void)
{
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        logError("Failed to create socket: %s", strerror(errno));
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(getServerPort());

    if (inet_pton(AF_INET, getServerIp(), &server_addr.sin_addr) <= 0)
    {
        logError("Invalid address/ Address not supported: %s", getServerIp());
        close(sockfd);
        return -1;
    }

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        logError("Failed to bind socket: %s", strerror(errno));
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, SOMAXCONN) < 0)
    {
        logError("Failed to listen on socket: %s", strerror(errno));
        close(sockfd);
        return -1;
    }

    logInfo("Socket created and bound to %s:%d", getServerIp(), getServerPort());
    return sockfd;
}

/**
 * \brief Accepts a new connection on the given socket.
 *
 * \param sockfd The socket file descriptor.
 * \return The file descriptor for the accepted connection, or -1 on failure.
 */
int acceptConnection(int sockfd)
{
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        logError("Failed to accept connection: %s", strerror(errno));
        return -1;
    }

    logInfo("Accepted connection from %s:%d",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    return client_fd;
}
