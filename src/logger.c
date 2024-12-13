/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* logger.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../include/logger.h"

static FILE *log_file = NULL;
static int current_log_level = LOG_LEVEL_INFO;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

int logInit(const char *filename, int log_level)
{
    pthread_mutex_lock(&log_mutex);

    if (log_file) {
        fclose(log_file);
    }

    log_file = fopen(filename, "a");
    if (!log_file) {
        pthread_mutex_unlock(&log_mutex);
        return -1;
    }

    /* Set file permissions to 600 */
    chmod(filename, 0600);

    current_log_level = log_level;

    pthread_mutex_unlock(&log_mutex);
    return 0;
}

static void logMessage(int level, const char *format, va_list args)
{
    time_t now;
    struct tm *tm_info;
    char timestamp[26];
    char *level_str;

    if (level > current_log_level || !log_file) {
        return;
    }

    pthread_mutex_lock(&log_mutex);

    /* Get current time */
    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    /* Determine log level string */
    switch (level) {
        case LOG_LEVEL_ERROR:
            level_str = "ERROR";
            break;
        case LOG_LEVEL_WARN:
            level_str = "WARN";
            break;
        case LOG_LEVEL_INFO:
            level_str = "INFO";
            break;
        case LOG_LEVEL_DEBUG:
            level_str = "DEBUG";
            break;
        default:
            level_str = "UNKNOWN";
    }

    /* Write log entry */
    fprintf(log_file, "[%s] [%s] ", timestamp, level_str);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    fflush(log_file);

    pthread_mutex_unlock(&log_mutex);
}

void logError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_ERROR, format, args);
    va_end(args);
}

void logWarn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_WARN, format, args);
    va_end(args);
}

void logInfo(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_INFO, format, args);
    va_end(args);
}

void logDebug(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_DEBUG, format, args);
    va_end(args);
}

void logCleanup(void)
{
    pthread_mutex_lock(&log_mutex);
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
    pthread_mutex_unlock(&log_mutex);
}
