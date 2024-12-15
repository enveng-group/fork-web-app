/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/logging.c */
#include "../include/logging.h"
#include "../include/constants.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>

/* Static variables */
static FILE *log_fp = NULL;
static char log_file_path[MAX_LINE_LENGTH];
static long current_log_size = 0;

/* Log level strings */
static const char *log_level_str[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL"
};

/* Internal helper functions */
static int openLogFile(void);
static int writeLogEntry(enum log_level level, const char *format, va_list args);
static void formatTimestamp(char *buffer, size_t size);
static int checkLogSize(void);

int
logInit(const char *log_path)
{
    mode_t old_umask;

    if (log_path == NULL) {
        errorLog(ERROR_WARNING, "Log path is NULL, using stderr");
        log_fp = stderr;
        return -1;
    }

    /* Store log path */
    strncpy(log_file_path, log_path, MAX_LINE_LENGTH - 1);
    log_file_path[MAX_LINE_LENGTH - 1] = '\0';

    /* Set restrictive permissions for log file */
    old_umask = umask(077);

    /* Open log file */
    if (openLogFile() != 0) {
        umask(old_umask);
        return -1;
    }

    /* Restore original umask */
    umask(old_umask);

    /* Get initial file size */
    if (fseek(log_fp, 0, SEEK_END) == 0) {
        current_log_size = ftell(log_fp);
        if (current_log_size == -1) {
            current_log_size = 0;
        }
    }

    return 0;
}

void
logWrite(enum log_level level, const char *format, ...)
{
    va_list args;

    if (format == NULL || level < LOG_DEBUG || level > LOG_FATAL) {
        return;
    }

    /* Check if log needs rotation */
    if (checkLogSize() != 0) {
        logRotate();
    }

    va_start(args, format);
    writeLogEntry(level, format, args);
    va_end(args);
}

void
logRotate(void)
{
    char backup_path[MAX_LINE_LENGTH];
    time_t now;
    struct tm *tm_info;
    char timestamp[32];

    if (log_fp == NULL || log_fp == stderr) {
        return;
    }

    /* Close current log file */
    fclose(log_fp);

    /* Generate backup filename with timestamp */
    time(&now);
    tm_info = localtime(&now);
    if (tm_info == NULL) {
        errorLog(ERROR_WARNING, "Failed to get local time for log rotation");
        openLogFile();
        return;
    }

    if (strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_info) == 0) {
        errorLog(ERROR_WARNING, "Failed to format timestamp for log rotation");
        openLogFile();
        return;
    }

    snprintf(backup_path, sizeof(backup_path), "%s.%s", log_file_path, timestamp);

    /* Rename current log to backup */
    if (rename(log_file_path, backup_path) != 0 && errno != ENOENT) {
        errorLog(ERROR_WARNING, "Failed to rename log file during rotation");
    }

    /* Open new log file */
    openLogFile();
    current_log_size = 0;
}

void
logCleanup(void)
{
    if (log_fp != NULL && log_fp != stderr) {
        fclose(log_fp);
        log_fp = NULL;
    }
    current_log_size = 0;
}

static int
openLogFile(void)
{
    log_fp = fopen(log_file_path, "a");
    if (log_fp == NULL) {
        errorLog(ERROR_WARNING, "Failed to open log file, using stderr");
        log_fp = stderr;
        return -1;
    }
    return 0;
}

static int
writeLogEntry(enum log_level level, const char *format, va_list args)
{
    char timestamp[32];
    int written;
    va_list args_copy;

    if (log_fp == NULL) {
        return -1;
    }

    /* Format timestamp */
    formatTimestamp(timestamp, sizeof(timestamp));

    /* Write log entry header */
    written = fprintf(log_fp, "[%s] [%s] ", timestamp, log_level_str[level]);
    if (written < 0) {
        return -1;
    }
    current_log_size += written;

    /* Write formatted message */
    va_copy(args_copy, args);
    written = vfprintf(log_fp, format, args_copy);
    va_end(args_copy);

    if (written < 0) {
        return -1;
    }
    current_log_size += written;

    /* Add newline */
    if (fputc('\n', log_fp) == EOF) {
        return -1;
    }
    current_log_size++;

    fflush(log_fp);
    return 0;
}

static void
formatTimestamp(char *buffer, size_t size)
{
    time_t now;
    struct tm *tm_info;

    time(&now);
    tm_info = localtime(&now);

    if (tm_info == NULL) {
        strncpy(buffer, "TIMESTAMP_ERROR", size - 1);
        buffer[size - 1] = '\0';
        return;
    }

    if (strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info) == 0) {
        strncpy(buffer, "TIMESTAMP_ERROR", size - 1);
        buffer[size - 1] = '\0';
    }
}

static int
checkLogSize(void)
{
    return (current_log_size >= MAX_LOG_SIZE);
}
