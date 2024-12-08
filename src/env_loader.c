/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "../include/env_loader.h"
#include "../include/constants.h"
#include "../include/logger.h"
#include "../include/utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    LINE_SIZE = 256
};

static char *
trim (char *str)
{
    char *end;

    /* Trim leading space */
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  /* All spaces? */
        return str;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    /* Write new null terminator */
    *(end + 1) = '\0';

    return str;
}

int loadEnvConfig(const char *filename) {
    FILE *file;
    char line[256];

    file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        return -1;  /* Change return type to int and return -1 on error */
    }

    while (fgets(line, sizeof(line), file)) {
        char *trimmed_line = trim(line);
        /* Process trimmed_line */
        printf("Processed line: %s\n", trimmed_line); /* Example usage */
    }

    fclose(file);
    return 0;  /* Return 0 on success */
}

void
loadEnvironmentVariables (void)
{
    FILE *file;
    char line[LINE_SIZE];
    char *key;
    char *value;
    char *saveptr = NULL;

    logInfo("Attempting to open environment file: %s", ENV_FILE);
    file = fopen(ENV_FILE, "r");
    if (!file)
    {
        logError("Failed to open environment file: %s", ENV_FILE);
        return;
    }

    logInfo("Environment file opened successfully: %s", ENV_FILE);
    while (fgets(line, sizeof(line), file))
    {
        char *trimmed_line = trimWhitespace(line);
        if (isEmptyOrComment(trimmed_line))
        {
            continue;
        }

        key = strtok_r(trimmed_line, "=", &saveptr);
        value = strtok_r(NULL, "\n", &saveptr);
        if (key && value)
        {
            key = trimWhitespace(key);
            value = trimWhitespace(value);

            if (strcmp(key, "SERVER_IP") == 0)
            {
                safeStrncpy(SERVER_IP, value, sizeof(SERVER_IP));
                logInfo("Loaded SERVER_IP: %s", SERVER_IP);
            }
            else if (strcmp(key, "SERVER_PORT") == 0)
            {
                SERVER_PORT = atoi(value);
                logInfo("Loaded SERVER_PORT: %d", SERVER_PORT);
            }
            else if (strcmp(key, "SSL_CERT_FILE") == 0)
            {
                safeStrncpy(SSL_CERT_FILE, value, sizeof(SSL_CERT_FILE));
                logInfo("Loaded SSL_CERT_FILE: %s", SSL_CERT_FILE);
            }
            else if (strcmp(key, "SSL_KEY_FILE") == 0)
            {
                safeStrncpy(SSL_KEY_FILE, value, sizeof(SSL_KEY_FILE));
                logInfo("Loaded SSL_KEY_FILE: %s", SSL_KEY_FILE);
            }
            else
            {
                logWarning("Unknown environment variable: %s", key);
            }
        }
        else
        {
            logError("Failed to parse environment line: %s", line);
        }
    }

    fclose(file);
    logInfo("Environment variables loaded successfully");
}
