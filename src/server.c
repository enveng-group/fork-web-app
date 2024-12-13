/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* System includes */
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

/* Project includes */
#include "../include/server.h"
#include "../include/config.h"
#include "../include/logger.h"
#include "../include/http.h"
#include "../include/data_structures.h"
#include "../include/ssl_module.h"
#include "../include/quic_module.h"

#define MAX_CONNECTIONS 100
#define BUFFER_SIZE 1024

static volatile int server_running = 1;
static int server_socket = -1;

/* Use the ServerState struct defined in server.h */
struct ServerState server_state = {NULL, NULL};  /* Initialize with NULL values */

/* Signal handler for graceful shutdown */
static void handle_signal(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        server_running = 0;
        if (server_socket != -1)
        {
            close(server_socket);
        }
    }
}

/* Forward declarations */
static int handleClientRequest(SSL *ssl);

int initServer(const struct ServerConfig *config)
{
    SSL_CTX *ssl_ctx;
    struct sigaction sa;
    struct sockaddr_in server_addr;
    int optval = 1;

    /* Create SSL context */
    ssl_ctx = createSslContext(config->ssl_cert, config->ssl_key);
    if (!ssl_ctx) {
        logError("Failed to initialize SSL");
        return -1;
    }

    /* Initialize HTTP module with SSL context */
    httpInit(ssl_ctx);

    /* Initialize signal handling */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* Create socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        logError("Failed to create socket");
        return -1;
    }

    /* Set socket options */
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        logError("Failed to set socket options");
        close(server_socket);
        return -1;
    }

    /* Configure server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config->port);

    /* Bind socket */
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        logError("Failed to bind socket");
        close(server_socket);
        return -1;
    }

    /* Listen for connections */
    if (listen(server_socket, MAX_CONNECTIONS) == -1)
    {
        logError("Failed to listen on socket");
        close(server_socket);
        return -1;
    }

    /* Initialize QUIC context */
    server_state.quic_ctx = initializeQuicContext(server_socket,
                                                ssl_ctx,
                                                config->max_streams,
                                                config->quic_timeout);
    if (!server_state.quic_ctx) {
        logError("Failed to initialize QUIC context");
        cleanupSslContext(ssl_ctx);
        return -1;
    }

    server_state.ssl_ctx = ssl_ctx;
    return 0;
}

int runServer(void)
{
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    SSL *ssl;

    while (server_running)
    {
        client_socket = accept(server_socket,
                             (struct sockaddr *)&client_addr,
                             &client_len);

        if (client_socket < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            logError("Failed to accept connection");
            continue;
        }

        ssl = SSL_new(server_state.ssl_ctx);
        if (!ssl)
        {
            close(client_socket);
            continue;
        }

        SSL_set_fd(ssl, client_socket);

        if (SSL_accept(ssl) <= 0)
        {
            SSL_free(ssl);
            close(client_socket);
            continue;
        }

        handleClientRequest(ssl);

        SSL_free(ssl);
        close(client_socket);
    }

    return 0;
}

static int handleClientRequest(SSL *ssl)
{
    struct HttpRequest request;
    struct HttpResponse *response;
    int result;

    if (!ssl)
    {
        logError("Invalid SSL connection");
        return -1;
    }

    memset(&request, 0, sizeof(struct HttpRequest));

    /* Handle the request */
    result = httpHandleConnection(ssl);
    if (result != 0)
    {
        response = httpCreateResponse(HTTP_INTERNAL_ERROR, "text/plain");
        if (response)
        {
            httpSendResponse(ssl, response);
            httpFreeResponse(response);
        }
    }

    return result;
}

void stopServer(void)
{
    if (server_state.quic_ctx) {
        cleanupQuicContext(server_state.quic_ctx);
    }
    if (server_state.ssl_ctx) {
        cleanupSslContext(server_state.ssl_ctx);
    }
    if (server_socket != -1) {
        close(server_socket);
    }
}
