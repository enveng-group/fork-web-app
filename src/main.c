/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* src/main.c */
#include "../include/init.h"
#include "../include/fs.h"
#include "../include/app_error.h"
#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile sig_atomic_t running = 1;

static void
sigHandler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM)
    {
        running = 0;
    }
}

int
main(int argc, char *argv[])
{
    int status;
    struct sigaction sa;
    const char *log_path = LOG_PATH;

    /* Use argc/argv if specified */
    if (argc > 1)
    {
        log_path = argv[1];
    }

    /* Initialize signal handling */
    sa.sa_handler = sigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1 ||
        sigaction(SIGTERM, &sa, NULL) == -1)
    {
        fprintf(stderr, "Failed to set up signal handlers\n");
        return EXIT_FAILURE;
    }

    /* Initialize error handling with optional custom log path */
    errorInit(log_path);

    /* Initialize system */
    status = initSystem();
    if (status != INIT_SUCCESS)
    {
        errorLog(ERROR_CRITICAL, "System initialization failed");
        return EXIT_FAILURE;
    }

    /* Initialize filesystem */
    status = fsInit("/");
    if (status != FS_SUCCESS)
    {
        errorLog(ERROR_CRITICAL, "Filesystem initialization failed");
        shutdownSystem();
        return EXIT_FAILURE;
    }

    /* Initialize shell */
    status = shellInit();
    if (status != 0)
    {
        errorLog(ERROR_CRITICAL, "Shell initialization failed");
        shutdownSystem();
        return EXIT_FAILURE;
    }

    /* Main loop */
    while (running)
    {
        shellPrompt();
    }

    /* Cleanup */
    shellShutdown();
    shutdownSystem();
    errorShutdown();

    return EXIT_SUCCESS;
}
