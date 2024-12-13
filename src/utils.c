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
#include "../include/utils.h"
#include "../include/logger.h"

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
