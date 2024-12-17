/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/config.c */
/* System includes */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <limits.h>

/* Local includes */
#include "../include/config.h"
#include "../include/constants.h"
#include "../include/app_error.h"
#include "../include/cache.h"

/* Internal constants */
#define MAX_CONFIG_ENTRIES 100
#define MAX_LINE_LENGTH (MAX_ENV_KEY + MAX_ENV_VALUE + 2)

/* Internal storage */
static struct {
    char key[MAX_ENV_KEY];
    char value[MAX_ENV_VALUE];
} config_entries[MAX_CONFIG_ENTRIES];

static size_t num_entries = 0;
static int is_initialized = 0;
static enum config_status current_status = CONFIG_ERROR;

/* Internal functions */
static int parse_line(const char *line, char *key, char *value)
{
    const char *delimiter;
    size_t key_len;
    size_t value_len;

    if (line == NULL || key == NULL || value == NULL) {
        return -1;
    }

    delimiter = strchr(line, '=');
    if (delimiter == NULL) {
        return -1;
    }

    key_len = (size_t)(delimiter - line);
    value_len = strlen(delimiter + 1);

    if (key_len == 0 || key_len >= MAX_ENV_KEY || value_len >= MAX_ENV_VALUE) {
        return -1;
    }

    memcpy(key, line, key_len);
    key[key_len] = '\0';

    strcpy(value, delimiter + 1);
    value[value_len] = '\0';

    return 0;
}

int configInit(const char *config_path)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    char *newline;

    if (config_path == NULL) {
        current_status = CONFIG_ERROR;
        return -1;
    }

    /* Reset state */
    num_entries = 0;
    is_initialized = 0;

    fp = fopen(config_path, "r");
    if (fp == NULL) {
        current_status = CONFIG_FILE_ERROR;
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL && num_entries < MAX_CONFIG_ENTRIES) {
        /* Remove trailing newline if present */
        newline = strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        if (parse_line(line, config_entries[num_entries].key,
                      config_entries[num_entries].value) == 0) {
            num_entries++;
        }
    }

    fclose(fp);
    is_initialized = 1;
    current_status = CONFIG_SUCCESS;
    return 0;
}

int configGet(const char *key, char *value, size_t size)
{
    size_t i;

    if (!is_initialized || key == NULL || value == NULL || size == 0) {
        return -1;
    }

    for (i = 0; i < num_entries; i++) {
        if (strcmp(config_entries[i].key, key) == 0) {
            if (strlen(config_entries[i].value) >= size) {
                return -1;
            }
            strcpy(value, config_entries[i].value);
            return 0;
        }
    }

    return -1;
}

void configCleanup(void)
{
    num_entries = 0;
    is_initialized = 0;
    current_status = CONFIG_ERROR;
}

enum config_status configGetStatus(void)
{
    return current_status;
}
