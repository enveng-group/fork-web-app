/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* logger.h */
#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>
#include <stdarg.h>

/* Log levels */
enum LogLevel {
    LOG_LEVEL_ERROR = LOG_ERR,
    LOG_LEVEL_WARN = LOG_WARNING,
    LOG_LEVEL_INFO = LOG_INFO,
    LOG_LEVEL_DEBUG = LOG_DEBUG
};

/* Initialize logging */
int logInit(const char *log_file, int log_level);

/* Log functions for different levels */
void logError(const char *format, ...);
void logWarn(const char *format, ...);
void logInfo(const char *format, ...);
void logDebug(const char *format, ...);

/* Close logging */
void logCleanup(void);

#endif /* LOGGER_H */
