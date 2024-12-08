/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "../include/logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* logger.c */
void
initLogger(void)
{
    /* Initialize logging system */
}

/* Log a message */
void
logMessage(const char *message)
{
    if (message == NULL)
    {
        fprintf(stderr, "Error: NULL message passed to logMessage\n");
        return;
    }
    printf("%s\n", message);
}

/* Log an error message */
void
logError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

/* Log an info message */
void
logInfo(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

/* Log a warning message */
void
logWarning(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
