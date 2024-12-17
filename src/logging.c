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
#include <pthread.h>

/* File-level variables */
static char log_file_path[LOG_MAX_PATH_LEN];
static FILE *log_file = NULL;
static enum log_level current_level = LOG_INFO;
static enum log_flags current_flags = LOG_FLAG_NONE;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Internal function prototypes */
static int openLogFile(const char *path);
static int rotateLogIfNeeded(void);
static int writeLogEntry(enum log_level level, const char *msg);
static int copyLogPath(const char *src);

int
logInit(const char *log_path, enum log_flags flags)
{
    if (log_path == NULL) {
        return -1;
    }

    pthread_mutex_lock(&log_mutex);

    if (log_file != NULL) {
        pthread_mutex_unlock(&log_mutex);
        return -1;
    }

    if (openLogFile(log_path) != 0) {
        pthread_mutex_unlock(&log_mutex);
        return -1;
    }

    current_flags = flags;
    pthread_mutex_unlock(&log_mutex);
    return 0;
}

void
logWrite(enum log_level level, const char *format, ...)
{
    va_list args;
    char message[LOG_MAX_MSG_LEN];

    if (format == NULL || log_file == NULL) {
        return;
    }

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    pthread_mutex_lock(&log_mutex);
    rotateLogIfNeeded();
    writeLogEntry(level, message);
    pthread_mutex_unlock(&log_mutex);
}

void
logCleanup(void)
{
    pthread_mutex_lock(&log_mutex);
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }
    pthread_mutex_unlock(&log_mutex);
}

int
logSetLevel(enum log_level level)
{
    if (level < LOG_EMERG || level > LOG_DEBUG) {
        return -1;
    }
    current_level = level;
    return 0;
}

enum log_level
logGetLevel(void)
{
    return current_level;
}

const char *
logLevelToString(enum log_level level)
{
    static const char *level_strings[] = {
        "EMERG",
        "ALERT",
        "CRIT",
        "ERROR",
        "WARNING",
        "NOTICE",
        "INFO",
        "DEBUG"
    };

    if (level <= LOG_DEBUG) {
        return level_strings[level];
    }
    return "UNKNOWN";
}

enum log_level
logStringToLevel(const char *level_str)
{
    if (level_str == NULL) {
        return LOG_INFO;  /* Default level */
    }

    if (strcmp(level_str, "DEBUG") == 0) return LOG_DEBUG;
    if (strcmp(level_str, "INFO") == 0) return LOG_INFO;
    if (strcmp(level_str, "NOTICE") == 0) return LOG_NOTICE;
    if (strcmp(level_str, "WARNING") == 0) return LOG_WARNING;
    if (strcmp(level_str, "ERROR") == 0) return LOG_ERR;
    if (strcmp(level_str, "CRIT") == 0) return LOG_CRIT;
    if (strcmp(level_str, "ALERT") == 0) return LOG_ALERT;
    if (strcmp(level_str, "EMERG") == 0) return LOG_EMERG;

    return LOG_INFO;  /* Default level */
}

/* Internal helper functions */
static int
copyLogPath(const char *src)
{
    size_t len;
    char temp[LOG_MAX_PATH_LEN];

    if (src == NULL) {
        return -1;
    }

    len = strlen(src);
    if (len >= LOG_MAX_PATH_LEN) {
        return -1;
    }

    /* First copy to temporary buffer to avoid overlap */
    memcpy(temp, src, len);
    temp[len] = '\0';

    /* Then copy to destination */
    memcpy(log_file_path, temp, len + 1);

    return 0;
}

static int
openLogFile(const char *path)
{
    FILE *new_file;

    if (path == NULL) {
        return -1;
    }

    /* Open file first */
    new_file = fopen(path, "a");
    if (new_file == NULL) {
        return -1;
    }

    /* Set permissions */
    if (chmod(path, LOG_DEFAULT_MODE) != 0) {
        fclose(new_file);
        return -1;
    }

    /* Store file handle */
    log_file = new_file;

    /* Copy path safely */
    if (copyLogPath(path) != 0) {
        fclose(log_file);
        log_file = NULL;
        return -1;
    }

    return 0;
}

/* Static function to write log entry with proper formatting */
static int
writeLogEntry(enum log_level level, const char *msg)
{
    time_t now;
    struct tm *timeinfo;
    char timestamp[32];

    if (log_file == NULL || msg == NULL) {
        return -1;
    }

    /* Check log level */
    if (level > current_level) {
        return 0;
    }

    /* Get current time */
    now = time(NULL);
    if (now == (time_t)-1) {
        return -1;
    }

    timeinfo = localtime(&now);
    if (timeinfo == NULL) {
        return -1;
    }

    /* Format timestamp */
    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo) == 0) {
        return -1;
    }

    /* Write log entry with timestamp and level */
    if (fprintf(log_file, "[%s] [%s] %s\n",
               timestamp,
               logLevelToString(level),
               msg) < 0) {
        return -1;
    }

    /* Flush to disk immediately for reliability */
    if (fflush(log_file) != 0) {
        return -1;
    }

    return 0;
}

/* Static function to check and rotate logs if needed */
static int
rotateLogIfNeeded(void)
{
    struct stat st;
    char new_name[LOG_MAX_PATH_LEN];
    char temp_path[LOG_MAX_PATH_LEN];
    int fd;
    time_t now;
    size_t path_len;

    if (log_file == NULL) {
        return -1;
    }

    fd = fileno(log_file);
    if (fd == -1) {
        return -1;
    }

    if (fstat(fd, &st) != 0) {
        return -1;
    }

    if (st.st_size >= LOG_MAX_SIZE) {
        now = time(NULL);
        if (now == (time_t)-1) {
            return -1;
        }

        /* Save current path before closing */
        path_len = strlen(log_file_path);
        if (path_len >= LOG_MAX_PATH_LEN) {
            return -1;
        }
        strncpy(temp_path, log_file_path, LOG_MAX_PATH_LEN - 1);
        temp_path[LOG_MAX_PATH_LEN - 1] = '\0';

        fclose(log_file);
        log_file = NULL;

        /* Generate new name with timestamp */
        if (snprintf(new_name, sizeof(new_name), "%s.%ld",
                    temp_path, (long)now) >= (int)sizeof(new_name)) {
            return -1;
        }

        /* Rotate file */
        if (rename(temp_path, new_name) != 0) {
            return -1;
        }

        /* Reopen log file */
        return openLogFile(temp_path);
    }

    return 0;
}

/* Add missing logging rotation function */
int
logRotate(void)
{
    char backup_path[LOG_MAX_PATH_LEN];
    char temp_path[LOG_MAX_PATH_LEN];
    time_t now;
    struct tm *tm_info;
    int result;
    mode_t old_umask;
    FILE *new_file;

    /* Verify log file exists */
    if (log_file == NULL) {
        return -1;
    }

    pthread_mutex_lock(&log_mutex);

    /* Store current path */
    strncpy(temp_path, log_file_path, sizeof(temp_path) - 1);
    temp_path[sizeof(temp_path) - 1] = '\0';

    /* Close current log file */
    fclose(log_file);
    log_file = NULL;

    /* Generate backup filename with timestamp */
    time(&now);
    tm_info = localtime(&now);
    if (tm_info == NULL) {
        pthread_mutex_unlock(&log_mutex);
        return -1;
    }

    result = snprintf(backup_path, sizeof(backup_path),
                     "%s.%04d%02d%02d-%02d%02d%02d",
                     temp_path,
                     tm_info->tm_year + 1900,
                     tm_info->tm_mon + 1,
                     tm_info->tm_mday,
                     tm_info->tm_hour,
                     tm_info->tm_min,
                     tm_info->tm_sec);

    if (result < 0 || (size_t)result >= sizeof(backup_path)) {
        pthread_mutex_unlock(&log_mutex);
        return -1;
    }

    /* Rename current log file to backup */
    if (rename(temp_path, backup_path) != 0 && errno != ENOENT) {
        pthread_mutex_unlock(&log_mutex);
        return -1;
    }

    /* Create new log file with proper permissions */
    old_umask = umask(077);
    new_file = fopen(temp_path, "a");
    umask(old_umask);

    if (new_file == NULL) {
        pthread_mutex_unlock(&log_mutex);
        return -1;
    }

    /* Store new file handle */
    log_file = new_file;

    pthread_mutex_unlock(&log_mutex);
    return 0;
}
