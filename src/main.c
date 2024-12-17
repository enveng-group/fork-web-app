/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* src/main.c */
#include "../include/init.h"
#include "../include/fs.h"
#include "../include/app_error.h"
#include "../include/shell.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/mem.h"
#include "../include/cache.h"
#include "../include/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

static volatile sig_atomic_t running = 1;

static void
sigHandler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM)
    {
        running = 0;
    }
}

static int
initializeSubsystems(const char *log_path)
{
    int status;
    struct sigaction sa;

    /* Initialize error handling first */
    errorInit(log_path);

    /* Set up signal handlers */
    sa.sa_handler = sigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1 ||
        sigaction(SIGTERM, &sa, NULL) == -1)
    {
        errorLog(ERROR_CRITICAL, "Failed to set up signal handlers");
        return -1;
    }

    /* Initialize core subsystems in correct order */
    if ((status = constants_init()) != 0) {
        errorLog(ERROR_CRITICAL, "Failed to initialize constants");
        return -1;
    }

    if ((status = memInit(MEM_POOL_SIZE)) != 0) {
        errorLog(ERROR_CRITICAL, "Failed to initialize memory management");
        return -1;
    }

    if ((status = cacheInit(CACHE_TYPE_LRU, CACHE_MAX_ENTRIES)) != 0) {
        errorLog(ERROR_CRITICAL, "Failed to initialize cache");
        memCleanup();
        return -1;
    }

    if ((status = fsInit("/")) != FS_SUCCESS) {
        errorLog(ERROR_CRITICAL, "Failed to initialize filesystem");
        cacheCleanup();
        memCleanup();
        return -1;
    }

    if ((status = processInit()) != 0) {
        errorLog(ERROR_CRITICAL, "Failed to initialize process management");
        cacheCleanup();
        memCleanup();
        return -1;
    }

    if ((status = schedulerInit()) != SCHEDULER_SUCCESS) {
        errorLog(ERROR_CRITICAL, "Failed to initialize scheduler");
        processCleanup();
        cacheCleanup();
        memCleanup();
        return -1;
    }

    if ((status = shellInit()) != 0) {
        errorLog(ERROR_CRITICAL, "Failed to initialize shell");
        schedulerCleanup();
        processCleanup();
        cacheCleanup();
        memCleanup();
        return -1;
    }

    if ((status = socketInit(SOCKET_INIT_DEFAULT)) != 0) {
        errorLog(ERROR_CRITICAL, "Failed to initialize socket subsystem");
        return -1;
    }

    return 0;
}

static void
cleanupSubsystems(void)
{
    shellShutdown();
    schedulerStop();
    schedulerCleanup();
    processCleanup();
    cacheCleanup();
    memCleanup();
    socketCleanup();
    shutdownSystem();
    errorShutdown();
}

int
main(int argc, char *argv[])
{
    const char *log_path = DEFAULT_LOG_PATH;
    int status;

    /* Use custom log path if provided */
    if (argc > 1) {
        log_path = argv[1];
    }

    /* Initialize all subsystems */
    status = initializeSubsystems(log_path);
    if (status != 0) {
        fprintf(stderr, "System initialization failed\n");
        return EXIT_FAILURE;
    }

    /* Start scheduler */
    status = schedulerStart();
    if (status != SCHEDULER_SUCCESS) {
        errorLog(ERROR_CRITICAL, "Failed to start scheduler");
        cleanupSubsystems();
        return EXIT_FAILURE;
    }

    /* Main event loop */
    while (running) {
        status = shellPrompt();
        if (status != 0) {
            errorLog(ERROR_WARNING, "Shell error occurred");
        }
    }

    /* Clean shutdown */
    cleanupSubsystems();
    return EXIT_SUCCESS;
}
