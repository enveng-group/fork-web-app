/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: src/app_error.c */
#include "../include/app_error.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Static variables */
static FILE *log_file = NULL;
static char last_error[MAX_ERROR_MSG];

/* Error level strings must match enum error_level order */
static const char *error_level_str[] = {
    "INFO",
    "WARNING",
    "CRITICAL"
};

void
errorInit(const char *log_path)
{
    mode_t old_umask;

    if (log_path == NULL) {
        log_file = stderr;
        return;
    }

    /* Set restrictive permissions for log file */
    old_umask = umask(077);

    log_file = fopen(log_path, "a");
    if (log_file == NULL) {
        log_file = stderr;
    }

    /* Restore original umask */
    umask(old_umask);

    /* Initialize last error buffer */
    memset(last_error, 0, sizeof(last_error));
}

void
errorLog(enum error_level level, const char *message)
{
    time_t now;
    char timestamp[32];
    struct tm *tm_info;

    if (log_file == NULL || message == NULL ||
        level < ERROR_INFO || level > ERROR_CRITICAL) {
        return;
    }

    /* Get current time */
    time(&now);
    tm_info = localtime(&now);
    if (tm_info == NULL) {
        return;
    }

    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info) == 0) {
        return;
    }

    /* Write to log file */
    if (fprintf(log_file, "[%s] %s: %s\n",
            timestamp,
            error_level_str[level],
            message) < 0) {
        return;
    }
    fflush(log_file);

    /* Store last error message */
    strncpy(last_error, message, MAX_ERROR_MSG - 1);
    last_error[MAX_ERROR_MSG - 1] = '\0';
}

void
errorShutdown(void)
{
    if (log_file != NULL && log_file != stderr) {
        fclose(log_file);
        log_file = NULL;
    }
    memset(last_error, 0, sizeof(last_error));
}

const char *
errorGetLast(void)
{
    return last_error;
}
