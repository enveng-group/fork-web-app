/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: src/init.c */
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include "../include/init.h"
#include "../include/app_error.h"

/* Static variables */
static enum system_state current_state = STATE_STARTUP;

/* Essential directories - relative paths */
static const char *system_dirs[] = {
    "etc",
    "etc/config",
    "var",
    "var/log",
    "var/run",
    NULL
};

/* Static function prototypes */
static int create_directories(void);

/* Create required directories */
static int create_directories(void)
{
    const char **dir;
    mode_t old_umask;
    int status = 0;

    old_umask = umask(022);

    for (dir = system_dirs; *dir != NULL; dir++) {
        if (mkdir(*dir, 0755) != 0) {
            if (errno != EEXIST) {
                status = -1;
                break;
            }
        }
    }

    umask(old_umask);
    return status;
}

/* Initialize the system */
int initSystem(void)
{
    if (current_state == STATE_RUNNING) {
        return INIT_ERR_ALREADY_RUNNING;
    }

    if (create_directories() != 0) {
        current_state = STATE_ERROR;
        return INIT_ERROR;
    }

    current_state = STATE_RUNNING;
    return INIT_SUCCESS;
}

/* Shutdown the system */
int shutdownSystem(void)
{
    current_state = STATE_SHUTDOWN;
    return INIT_SUCCESS;
}

/* Get current system state */
enum system_state getSystemState(void)
{
    return current_state;
}
