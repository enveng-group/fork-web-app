/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/logging.h */
#ifndef LOGGING_H
#define LOGGING_H

#include <sys/types.h>

/* Log levels */
enum log_level {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
};

/* Function prototypes */
int logInit(const char *log_path);
void logWrite(enum log_level level, const char *format, ...);
void logRotate(void);
void logCleanup(void);

#endif /* LOGGING_H */
