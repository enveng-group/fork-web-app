/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: /devcontainer/web-app/include/app_error.h */
#ifndef APP_ERROR_H
#define APP_ERROR_H

/* Dependencies */
#include "common.h"

/* Documentation for dependent modules:
 * - Used by fs.h for error logging
 * - Used by scheduler.h for error reporting
 * - Core error handling facility
 */

/* Constants */
#define MAX_ERROR_MSG 256

/* Error levels */
enum error_level {
    ERROR_INFO = 0,
    ERROR_WARNING = 1,
    ERROR_CRITICAL = 2
};

/* Function prototypes */
void errorInit(const char *log_path);
void errorLog(enum error_level level, const char *message);
void errorShutdown(void);
const char *errorGetLast(void);

#endif /* APP_ERROR_H */
