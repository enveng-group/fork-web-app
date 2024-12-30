/* filepath: /home/appuser/fork-web-app/src/main.c */
/* C Standard Library headers */
#include <stdio.h>
#include <stdlib.h>

/* POSIX headers */
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>

/* Local headers */
#include "../include/web_server.h"

static volatile sig_atomic_t server_running = 1;

static void
signal_handler(int sig)
{
    if (sig == SIGTERM || sig == SIGINT) {
        server_running = 0;
    }
}

int
main(void)
{
    struct sigaction sa;
    int server_fd;
    int client_fd;

    /* Setup signal handler */
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) < 0 ||
        sigaction(SIGINT, &sa, NULL) < 0) {
        perror("Failed to setup signal handlers");
        return EXIT_FAILURE;
    }

    /* Start server */
    server_fd = setup_server(DEFAULT_PORT);
    if (server_fd < 0) {
        perror("Failed to setup server");
        return EXIT_FAILURE;
    }

    printf("Server running on port %d...\n", DEFAULT_PORT);

    /* Main server loop */
    while (server_running) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("Failed to accept connection");
            continue;
        }

        handle_client(client_fd, WWW_ROOT);
        close(client_fd);
    }

    /* Cleanup */
    close(server_fd);
    return EXIT_SUCCESS;
}
