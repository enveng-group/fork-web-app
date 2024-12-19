/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: /home/appuser/web-app/src/main.c */
#include "../include/web_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  /* For memset */
#include <signal.h>
#include <errno.h>   /* For errno and EINTR */
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

static volatile sig_atomic_t server_running = 1;
static int server_socket = -1;

static void
cleanup(void)
{
    if (server_socket != -1) {
        close(server_socket);
        server_socket = -1;
    }
}

static void
signal_handler(int sig)
{
    switch (sig) {
        case SIGTERM:
        case SIGINT:
            server_running = 0;
            break;
        default:
            break;
    }
}

static int
setup_signals(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGTERM, &sa, NULL) == -1 ||
        sigaction(SIGINT, &sa, NULL) == -1) {
        return -1;
    }

    return 0;
}

int
main(void)
{
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int client_socket;
    int server_fd;

    if (setup_signals() < 0) {
        perror("Failed to setup signals");
        return EXIT_FAILURE;
    }

    server_fd = setup_server(DEFAULT_PORT);
    if (server_fd < 0) {
        perror("Failed to setup server");
        return EXIT_FAILURE;
    }

    server_socket = server_fd;
    printf("Server running on port %d...\n", DEFAULT_PORT);

    while (server_running) {
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket,
                             (struct sockaddr *)&client_addr,
                             &client_len);

        if (client_socket < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("Failed to accept connection");
            continue;
        }

        if (handle_client(client_socket, WWW_ROOT) < 0) {
            /* Error already handled in handle_client */
            close(client_socket);
            continue;
        }
        close(client_socket);
    }

    cleanup();
    return EXIT_SUCCESS;
}
