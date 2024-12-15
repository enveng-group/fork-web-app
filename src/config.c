/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/config.c */
#include "../include/config.h"
#include "../include/constants.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Configuration limits */
#define CONFIG_MAX_VARS 100
#define CONFIG_PATH_MAX 4096

/* Static variables */
static char config_vars[CONFIG_MAX_VARS][MAX_ENV_VALUE];
static char config_keys[CONFIG_MAX_VARS][MAX_ENV_NAME];
static size_t config_count = 0;
static char config_file_path[CONFIG_PATH_MAX];
static enum config_status last_status = CONFIG_SUCCESS;

/* Internal helper functions */
static int parseConfigLine(const char *line, char *key, char *value);
static int findConfigKey(const char *key);

int
configInit(const char *config_path)
{
    /* Validate parameters */
    if (config_path == NULL)
    {
        last_status = CONFIG_FILE_ERROR;
        return -1;
    }

    /* Store config path */
    strncpy(config_file_path, config_path, CONFIG_PATH_MAX - 1);
    config_file_path[CONFIG_PATH_MAX - 1] = '\0';

    /* Reset configuration storage */
    memset(config_vars, 0, sizeof(config_vars));
    memset(config_keys, 0, sizeof(config_keys));
    config_count = 0;

    last_status = CONFIG_SUCCESS;
    return 0;
}

int
configLoad(void)
{
    FILE *config_file;
    char line[MAX_CONFIG_LINE];
    char key[MAX_ENV_NAME];
    char value[MAX_ENV_VALUE];

    config_file = fopen(config_file_path, "r");
    if (config_file == NULL)
    {
        errorLog(ERROR_WARNING, "Failed to open configuration file");
        last_status = CONFIG_FILE_ERROR;
        return -1;
    }

    while (fgets(line, sizeof(line), config_file) != NULL &&
           config_count < CONFIG_MAX_VARS)
    {
        /* Skip empty lines and comments */
        if (line[0] == '\n' || line[0] == '#')
        {
            continue;
        }

        /* Parse and store configuration */
        if (parseConfigLine(line, key, value) == 0)
        {
            strncpy(config_keys[config_count], key, MAX_ENV_NAME - 1);
            config_keys[config_count][MAX_ENV_NAME - 1] = '\0';
            strncpy(config_vars[config_count], value, MAX_ENV_VALUE - 1);
            config_vars[config_count][MAX_ENV_VALUE - 1] = '\0';
            config_count++;
        }
    }

    fclose(config_file);
    last_status = CONFIG_SUCCESS;
    return 0;
}

int
configGet(const char *key, char *value, size_t size)
{
    int index;

    /* Validate parameters */
    if (key == NULL || value == NULL)
    {
        last_status = CONFIG_ERROR;
        return -1;
    }

    /* Find configuration key */
    index = findConfigKey(key);
    if (index == -1)
    {
        last_status = CONFIG_NOT_FOUND;
        return -1;
    }

    /* Copy value safely */
    strncpy(value, config_vars[index], size - 1);
    value[size - 1] = '\0';

    last_status = CONFIG_SUCCESS;
    return 0;
}

void
configCleanup(void)
{
    /* Clear configuration data */
    memset(config_vars, 0, sizeof(config_vars));
    memset(config_keys, 0, sizeof(config_keys));
    memset(config_file_path, 0, sizeof(config_file_path));
    config_count = 0;
    last_status = CONFIG_SUCCESS;
}

enum config_status
configGetStatus(void)
{
    return last_status;
}

static int
parseConfigLine(const char *line, char *key, char *value)
{
    char *eq_pos;
    size_t key_len;

    /* Find equals sign */
    eq_pos = strchr(line, '=');
    if (eq_pos == NULL)
    {
        return -1;
    }

    /* Extract key */
    key_len = eq_pos - line;
    if (key_len >= MAX_ENV_NAME)
    {
        return -1;
    }
    strncpy(key, line, key_len);
    key[key_len] = '\0';

    /* Extract value */
    eq_pos++; /* Skip '=' */
    strncpy(value, eq_pos, MAX_ENV_VALUE - 1);
    value[MAX_ENV_VALUE - 1] = '\0';

    /* Remove trailing newline if present */
    key_len = strlen(value);
    if (key_len > 0 && value[key_len - 1] == '\n')
    {
        value[key_len - 1] = '\0';
    }

    return 0;
}

static int
findConfigKey(const char *key)
{
    size_t i;

    for (i = 0; i < config_count; i++)
    {
        if (strcmp(config_keys[i], key) == 0)
        {
            return (int)i;
        }
    }

    return -1;
}

int config_init(const char *filename)
{
    if (configInit(filename) != 0) {
        return -1;
    }
    return configLoad();
}

int config_get(const char *key, char *value, size_t value_size)
{
    return configGet(key, value, value_size);
}

void config_cleanup(void)
{
    configCleanup();
}
