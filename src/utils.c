/* filepath: src/utils.c */
/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <limits.h>
#include "../include/utils.h"
#include "../include/logger.h"

#define PATH_INVALID -1
#define PATH_NOT_FOUND -2
#define PATH_NO_ACCESS -3
#define PATH_OK 0

const char *REQUIRED_DIRS[] = {
    "../etc/env",
    "../etc/ssl",
    "../etc/auth",
    "../var/log",
    "../var/lib",
    NULL
};

char* generateUUID(void)
{
    static char uuid[37];
    const char* hex = "0123456789abcdef";
    int i;

    srand((unsigned int)time(NULL) ^ (unsigned int)getpid());

    for (i = 0; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            uuid[i] = '-';
        } else {
            uuid[i] = hex[rand() % 16];
        }
    }
    uuid[36] = '\0';

    return uuid;
}

int createDirectoryIfNotExists(const char *path)
{
    struct stat st;

    if (!path) {
        logError("Invalid path provided");
        return -1;
    }

    if (stat(path, &st) == -1) {
        if (mkdir(path, 0755) == -1) {
            logError("Failed to create directory: %s", path);
            return -1;
        }
    }

    return 0;
}

int fileExists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

int setFilePermissions(const char *path, mode_t mode)
{
    if (!path) {
        logError("Invalid path provided");
        return -1;
    }

    if (chmod(path, mode) == -1) {
        logError("Failed to set file permissions: %s", path);
        return -1;
    }

    return 0;
}

char* getCurrentUsername(void)
{
    struct passwd *pw;
    uid_t uid;

    uid = getuid();
    pw = getpwuid(uid);

    return pw ? pw->pw_name : NULL;
}

int validatePath(const char *path)
{
    if (!path || strlen(path) == 0) {
        return 0;
    }

    if (access(path, F_OK) == -1) {
        return 0;
    }

    return 1;
}

void secureZeroMemory(void *ptr, size_t size)
{
    volatile unsigned char *p = ptr;
    while (size--) {
        *p++ = 0;
    }
}

int validateRequiredPaths(void)
{
    const char *paths[] = {
        "../etc/env",
        "../etc/ssl",
        "../etc/auth",
        "../var/log",
        "../var/lib",
        NULL
    };
    int i;

    for (i = 0; paths[i] != NULL; i++) {
        if (!fileExists(paths[i])) {
            logError("Required directory not found: %s", paths[i]);
            return -1;
        }
    }
    return 0;
}

int validate_path(const char *path, int type)
{
    struct stat st;

    if (!path || strlen(path) == 0) {
        return PATH_INVALID;
    }

    if (stat(path, &st) == -1) {
        return PATH_NOT_FOUND;
    }

    if (type == 0 && !S_ISDIR(st.st_mode)) {
        return PATH_INVALID;
    }

    if (type == 1 && !S_ISREG(st.st_mode)) {
        return PATH_INVALID;
    }

    if (access(path, R_OK) == -1) {
        return PATH_NO_ACCESS;
    }

    return PATH_OK;
}

int validate_required_paths(const char *base_path)
{
    char full_path[PATH_MAX];
    int i;

    for (i = 0; REQUIRED_DIRS[i] != NULL; i++) {
        snprintf(full_path, sizeof(full_path), "%s/%s",
                base_path, REQUIRED_DIRS[i]);

        if (validate_path(full_path, 0) != PATH_OK) {
            logError("Required directory not found: %s", full_path);
            return -1;
        }
    }

    return 0;
}

static char*
trimWhitespace(char *str)
{
    char *end;

    while (*str && (*str == ' ' || *str == '\t'))
        str++;

    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n'))
        end--;

    *(end + 1) = '\0';
    return str;
}

int
parseEnvFile(const char *path, struct Config *config)
{
    FILE *fp;
    char line[ENV_MAX_LINE];
    char *key;
    char *value;
    char *ptr;

    if (!path || !config) {
        errno = EINVAL;
        return -1;
    }

    fp = fopen(path, "r");
    if (!fp) {
        logError("Failed to open env file: %s", path);
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n')
            continue;

        /* Split key=value */
        ptr = strchr(line, '=');
        if (!ptr)
            continue;

        *ptr = '\0';
        key = trimWhitespace(line);
        value = trimWhitespace(ptr + 1);

        /* Expand variables if needed */
        if (strstr(value, "${")) {
            char *expanded = expandEnvVars(value, config);
            if (expanded) {
                setEnvValue(config, key, expanded);
                free(expanded);
            }
        } else {
            setEnvValue(config, key, value);
        }
    }

    fclose(fp);
    return 0;
}

char*
expandEnvVars(const char *value, const struct Config *config)
{
    char *result;
    char *start;
    char *end;
    char var_name[ENV_MAX_KEY];
    size_t len;

    result = strdup(value);
    if (!result)
        return NULL;

    while ((start = strstr(result, "${")) && (end = strchr(start, '}'))) {
        *end = '\0';
        strncpy(var_name, start + 2, sizeof(var_name) - 1);

        /* Handle BASE_PATH special case */
        if (strcmp(var_name, "BASE_PATH") == 0) {
            len = strlen(config->base_path);
            memmove(start + len, end + 1, strlen(end + 1) + 1);
            memcpy(start, config->base_path, len);
        }
    }

    return result;
}

int
setEnvValue(struct Config *config, const char *key, const char *value)
{
    if (!config || !key || !value) {
        errno = EINVAL;
        return -1;
    }

    if (strcmp(key, "SERVER_PORT") == 0) {
        config->server_port = atoi(value);
    } else if (strcmp(key, "SERVER_HOST") == 0) {
        strncpy(config->server_host, value, sizeof(config->server_host) - 1);
    } else if (strcmp(key, "SSL_CERT_FILE") == 0) {
        strncpy(config->ssl_cert_file, value, sizeof(config->ssl_cert_file) - 1);
    }
    /* ...add other config variables... */

    return 0;
}
