/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef LOGGING_H
#define LOGGING_H

/* Dependencies - move constants.h include after common.h to avoid circular dependency */
#include "common.h"
#include "constants.h"

/* Documentation for dependent modules:
 * - Core logging facility
 * - Used by all modules for structured logging
 * - Provides log rotation and level filtering
 * - Thread-safe logging implementation
 */

/* Constants */
#define LOG_MAX_MSG_LEN 1024
#define LOG_MAX_PATH_LEN 256
#define LOG_MAX_FILES 10
#define LOG_DEFAULT_MODE 0644

/* Log rotation thresholds */
/* Remove duplicate LOG_MAX_SIZE definition and use the one from constants.h */
#define LOG_MAX_AGE (24 * 60 * 60)      /* 24 hours in seconds */

/* Log levels - compatible with syslog levels */
enum log_level {
    LOG_EMERG,   /* System is unusable */
    LOG_ALERT,   /* Action must be taken immediately */
    LOG_CRIT,    /* Critical conditions */
    LOG_ERR,     /* Error conditions */
    LOG_WARNING, /* Warning conditions */
    LOG_NOTICE,  /* Normal but significant condition */
    LOG_INFO,    /* Informational */
    LOG_DEBUG    /* Debug-level messages */
};

/* Log initialization flags */
enum log_flags {
    LOG_FLAG_NONE = 0,
    LOG_FLAG_TIMESTAMP = 1 << 0,
    LOG_FLAG_LEVEL = 1 << 1,
    LOG_FLAG_SYNC = 1 << 2,
    LOG_FLAG_PID = 1 << 3
};

/* Function prototypes */
int logInit(const char *log_path, enum log_flags flags);
void logWrite(enum log_level level, const char *format, ...);
int logRotate(void);
int logFlush(void);
void logCleanup(void);
const char *logLevelToString(enum log_level level);
enum log_level logStringToLevel(const char *level_str);
int logSetLevel(enum log_level level);
enum log_level logGetLevel(void);

#endif /* LOGGING_H */
