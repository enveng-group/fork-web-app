/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/init.c */
#include "../include/init.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

/* System state */
static enum system_state current_state = STATE_STARTUP;
static char env_vars[MAX_ENV_VARS][MAX_PATH_LEN];
static size_t env_count = 0;

int
initSystem(void)
{
    mode_t old_umask;

    /* Set restrictive umask */
    old_umask = umask(022);

    /* Set initial state */
    current_state = STATE_STARTUP;

    /* Initialize environment */
    if (loadConfiguration("/etc/system.conf") != INIT_SUCCESS)
    {
        /* Non-fatal error, continue initialization */
        errorLog(ERROR_WARNING, "Failed to load system configuration");
    }

    /* Set up system directories */
    if (mkdir("/var/run", 0755) == -1 && errno != EEXIST)
    {
        /* Non-fatal error, continue initialization */
        errorLog(ERROR_WARNING, "Failed to create runtime directory");
    }

    /* Restore original umask */
    umask(old_umask);

    /* Set running state */
    current_state = STATE_RUNNING;
    return INIT_SUCCESS;
}

int
shutdownSystem(void)
{
    /* Cleanup environment */
    env_count = 0;
    memset(env_vars, 0, sizeof(env_vars));

    current_state = STATE_SHUTDOWN;
    return INIT_SUCCESS;
}

int
rebootSystem(void)
{
    int status;

    status = shutdownSystem();
    if (status != INIT_SUCCESS)
    {
        return INIT_ERROR;
    }

    current_state = STATE_REBOOT;
    return INIT_SUCCESS;
}

int
loadConfiguration(const char *config_path)
{
    FILE *config;
    char line[MAX_PATH_LEN];
    char *ptr;

    if (config_path == NULL)
    {
        errno = EINVAL;
        return INIT_ERROR;
    }

    config = fopen(config_path, "r");
    if (config == NULL)
    {
        errorLog(ERROR_CRITICAL, "Could not open configuration file");
        return INIT_ERROR;
    }

    while (fgets(line, sizeof(line), config) != NULL && env_count < MAX_ENV_VARS)
    {
        /* Remove trailing newline */
        if ((ptr = strchr(line, '\n')) != NULL)
        {
            *ptr = '\0';
        }

        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#')
        {
            continue;
        }

        /* Store environment variable */
        strncpy(env_vars[env_count], line, MAX_PATH_LEN - 1);
        env_vars[env_count][MAX_PATH_LEN - 1] = '\0';
        env_count++;
    }

    fclose(config);
    return INIT_SUCCESS;
}

enum system_state
getSystemState(void)
{
    return current_state;
}
